// 1) Iterate over all files (id lists)
// 2) Build matrix of id inclusion at every list
//    id1 -> list1, list2, ... , listN
//    id2 -> list1, list2, ... , listN
//    ...
//    idM -> list1, list2, ... , listN
// 3) Iterate over all files at interview
// 4) For each uid find matched activity

// iterate over interview folder, collect ids to map type -> ids
// inerview_name -> uids vector -> activity_set

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <dirent.h>

#define MAX_BITS 64

using namespace std;

int  usage();
bool get_allfiles(string dir_name, vector<string>& files);

set<string> interviews_set;              // Уникальные uid'ы
map<string, set<string>> interviews_map; // файл -> uid'ы
map<string, array<int, MAX_BITS>> interviews_stat; // файл -> статистика

int main(int argc, char** argv) {

    if (argc < 3)
        return usage();

    // Путь к директории с файлами опроса
    string path = argv[1]; // interview ids
    // Файлы опроса
    vector<string> files;
    get_allfiles(path, files);
    sort(files.begin(), files.end());
    for (auto&& f : files)
        cerr << f << endl;

    for (auto&& f : files) {
        string   filename = path + "/" + f;
        ifstream file(filename);
        if (file.is_open()) {
            // инициализация статистики
            interviews_map[f] = set<string>{}; // Уникальные uid'ы файла
            interviews_stat[f] = array<int, MAX_BITS>{0}; // статистика файла

            string uid;
            // iterate throught lines
            while (getline(file, uid)) {
                interviews_set.insert(uid);      // Собираем уникальные uid'ы
                interviews_map[f].insert(uid);   // Собираем уникальные uid'ы в файле
                if (uid[uid.size() - 1] == '\r') // cleanup \r at the end
                    uid.resize(uid.size() - 1);
            }
        } else {
            cerr << "can't open " << filename << endl;
        }
    }

    // Папка с активностью пользователей
    files.clear();
    path = argv[2]; // activity ids
    get_allfiles(path, files);
    sort(files.begin(), files.end());
    for (auto&& f : files)
        cerr << f << endl;

    int bit = 0;
    // Проходим по всем файлам/датам
    for (auto&& f : files) {
        string   filename = path + "/" + f;
        ifstream file(filename);
        if (file.is_open()) {
            string uid;
            // Для каждого uid'a
            while (getline(file, uid)) {
                if (uid[uid.size() - 1] == '\r') // очищаем \r в конце
                    uid.resize(uid.size() - 1);
                // Проверяем был ли такой пользователь в опросе
                if (interviews_set.find(uid) != interviews_set.end()) {
                    // Если был, находим категорию ответа
                    for (auto&& i : interviews_map) {
                        // И инкрементируем её счетчик
                        if (i.second.find(uid) != i.second.end()) {
                            interviews_stat[i.first][bit]++;
                        }
                    }
                }
            }
            // следующая категория
            bit++;
        } else {
            cerr << "cant't open " << filename << endl;
        }
    }

    // пробегаем по уидам, выводим
    for (auto&& s : interviews_stat) {
        cout << s.first << " ";
        for (auto&& a : s.second) {
            cout << a << " ";
        }
        cout << endl;
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

// инструкция
int usage() {
    cout << "Usage:" << endl;
    cout << "\t./uids_activity interview_folder >> stat.txt" << endl;
    return 1;
}
