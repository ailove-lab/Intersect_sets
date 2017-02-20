#include <fstream>

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <dirent.h>

// Необходимо проверить процент актинвости пользователей из deep-activity в
// опросе
using namespace std;

// имя файла из каталога А -> множество ID
map<string, set<uint32_t>> folder_A_sets;
// имя файла из А + имя файла из B -> счетчик пересечений
map<string, int> intersections;

// инструкция
int usage() {
    cout << "Пересекает две группы множеств ID, каждое с каждым." << endl;
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

    if (!get_allfiles(argv[1], files_A) || !get_allfiles(argv[2], files_B))
        return 1;

    // пробегаем по каталогу А, для каждого файла создаем множестов ID
    for (auto&& f : files_A) {
        cerr << folder_A + "/" + f << endl;
        ifstream file(folder_A + "/" + f);
        if (file.is_open()) {
            string uid;
            while (getline(file, uid)) {
                if (uid[uid.size() - 1] == '\r')
                    uid.resize(uid.size() - 1);
                folder_A_sets[f].insert(stoi(uid));
            }
        }
    }

    // Выводим размеры множеств каталога А
    for (auto&& s : folder_A_sets) {
        cout << "A/" << s.first << "\t" << s.second.size() << endl;
    }

    // пробегаем по файлам из каталога B, считаем ко-во пересечений с
    // множествами каталога A
    for (auto&& f : files_B) {
        cerr << folder_B + "/" + f << endl;
        ifstream file(folder_B + "/" + f);
        if (file.is_open()) {
            string uid;
            int    c = 0;
            while (getline(file, uid)) {
                if (uid[uid.size() - 1] == '\r')
                    uid.resize(uid.size() - 1);
                uint32_t id = stoi(uid);
                for (auto&& s : folder_A_sets) {
                    string name = "A/" + s.first + "-" + "B/" + f;
                    if (s.second.find(id) != s.second.end())
                        intersections[name]++;
                }
                c++;
            }
            cout << "B/" << f << "\t" << c << endl;
        }
    }

    for (auto&& s : intersections) {
        cout << s.first << "\t" << s.second << endl;
    }

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
