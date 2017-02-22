// Построение модели пользователей
// 1) Пробегаем по группам опроса, собираем пользователей
//    all categories -> uids set
// 2) Пробегаем по датам активностей, собираем пользователей
//    all dates -> uids set
// 3) Пересекаем all categories с all dates
//    получаем множество активных пользователей
//    active uids -> uids set
// 4) Пробегаем по группам опроса, собираем только активных пользователей
//    category -> active uids set
// 5) Пробегаем по датам активности,
//    проверяем наличие в каждой активных пользователей
//    из всех категорий и общего списка
//    для каждого пользователя строим битовую маску активности
// 6) Собираем статистику по активности пользователей категории в заданный
// период

#include <fstream>

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <dirent.h>

using namespace std;

// имя файла из каталога А -> множество ID
map<string, set<uint32_t>> folder_A_sets;
// имя файла из А + имя файла из B -> счетчик пересечений
map<string, int> intersections;

// инструкция
int usage() {
    cout << "Строит модель поведения пользователей категорий А в категориях Б"
         << endl;
    cout << "Входные параметры - двa каталога, содержащие фалы с ID." << endl;
    cout << "Применение:" << endl;
    cout << "\t./intersect_sets folder_A folder_B >> intersection.txt" << endl;
    return 1;
}
bool get_allfiles(string dir_name, vector<string>& files);

// Пересечение двух групп множеств ID
int main(int argc, char** argv) {

    if (argc != 3)
        return usage();

    // загружаем списки файлов
    vector<string> files_A, files_B;
    string         folder_A = argv[1];
    string         folder_B = argv[2];

    // Получаем списки файлов в обрабатываемых каталогах
    if (!get_allfiles(argv[1], files_A) || !get_allfiles(argv[2], files_B))
        return 1;

    // пробегаем по каталогу А, для каждого файла создаем множество ID
    // all_uids_A
    set<int> all_uids_A;
    for (auto&& f : files_A) {
        cerr << folder_A + "/" + f << endl;
        ifstream file(folder_A + "/" + f);
        if (file.is_open()) {
            string uid;
            while (getline(file, uid)) {
                all_uids_A.insert(stoi(uid));
            }
            file.close();
        }
    }

    // пробегаем по каталогу B, для каждого файла создаем множество ID
    // all_uids_B
    set<int> all_uids_B;
    for (auto&& f : files_B) {
        cerr << folder_B + "/" + f << endl;
        ifstream file(folder_B + "/" + f);
        if (file.is_open()) {
            string uid;
            while (getline(file, uid)) {
                all_uids_B.insert(stoi(uid));
            }
            file.close();
        }
    }

    // Пересекаем all_uids_A с all_uids_B, строим active uids
    set<int> active_uids;
    for (auto&& a : all_uids_A) {
        if (all_uids_B.find(a) != all_uids_B.end())
            active_uids.insert(a);
    }
    cerr << "Всего найдено " << active_uids.size() << " пересечений." << endl;
    return 0;
}

// Получаем список файлов
bool get_allfiles(string dir_name, vector<string>& files) {
    DIR*           dir;
    struct dirent* ent;
    if ((dir = opendir(dir_name.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.')
                continue;
            files.push_back(ent->d_name);
        }
        closedir(dir);
        return 1;
    }

    cerr << "Не могу открыть каталог " << dir_name;
    return 0;
}
