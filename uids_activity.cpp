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

bool get_allfiles(string dir_name, vector<string>& files);

set<string> uids_set;                             // Уникальные uid'ы
map<string, set<string>> file_uids_set;           // файл -> uid'ы
map<string, array<int, MAX_BITS>> file_uids_stat; // файл -> статистика

// инструкция
int usage() {
    cout << "Usage:" << endl;
    cout << "\t./uids_activity interview_folder >> stat.txt" << endl;
    return 1;
}

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
            file_uids_set[f] = set<string>{}; // Уникальные uid'ы файла
            file_uids_stat[f] = array<int, MAX_BITS>{0}; // статистика файла

            string uid;
            // iterate throught lines
            while (getline(file, uid)) {
                uids_set.insert(uid); // Собираем уникальные uid'ы
                file_uids_set[f].insert(
                    uid); // Собираем уникальные uid'ы в файле
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
                if (uids_set.find(uid) != uids_set.end()) {
                    // Если был, находим категорию ответа
                    for (auto&& i : file_uids_set) {
                        // И инкрементируем её счетчик
                        if (i.second.find(uid) != i.second.end()) {
                            file_uids_stat[i.first][bit]++;
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
    for (auto&& s : file_uids_stat) {
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
