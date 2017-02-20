// Iterate over all files (id lists)
// build matrix of id inclusion at every list
// id1 -> list1, list2, ... , listN
// id2 -> list1, list2, ... , listN
// ...
// idM -> list1, list2, ... , listN

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <dirent.h>

#define MAX_BITS 64

using namespace std;

bool get_allfiles(string dir_name, vector<string>& files);

// инструкция
int usage(char* name) {
    cout << "Построение матрицы появления uid в файлах:" << endl;
    cout << "\t uid 00100110001" << endl << endl;
    cout << "Использование:" << endl;
    cout << "\t./" << name << " /folder>> stat.txt" << endl;
    return 1;
}

int main(int argc, char** argv) {

    if (argc < 2)
        return usage(argv[0]);

    string         path = argv[1];
    vector<string> files;

    get_allfiles(path, files);
    int files_count = files.size();
    // сортируем файлы по алфавиту
    sort(files.begin(), files.end());
    // выводим файлы
    for (auto&& f : files) {
        cerr << f << endl;
    }

    // uid -> битсет
    map<string, bitset<MAX_BITS>> uids;
    int bit = 0;
    // iterate throught files
    for (auto&& f : files) {
        string   filename = path + f;
        ifstream file(filename);
        if (file.is_open()) {
            string uid;
            // iterate throught lines
            while (getline(file, uid)) {
                if (uid[uid.size() - 1] == '\r')
                    uid.resize(uid.size() - 1);
                uids[uid][bit] = 1;
            }
            bit++;
        } else {
            cerr << "can't open " << filename << endl;
        }
    }

    // пробегаем по уидам, выводим
    for (auto&& u : uids) {
        cout << u.first << " " << u.second << endl;
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
