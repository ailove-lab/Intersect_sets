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
//    из всех категорий и общего списка
//    для каждого пользователя строим битовую маску активности
// 6) Выводим активность всех пользователей в sparse формате libsvm
//    category month_id:activity month_id:activity

#include <fstream>

#include <algorithm>
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
    cout << "Генерирует хронологию активности пользователей в формате libsvm"
         << endl;
    cout << "Входные параметры - двa каталога, содержащие фалы с ID." << endl;
    cout << "Применение:" << endl;
    cout << "\t./intersect_sets folder_A folder_B >> intersection.txt" << endl;
    return 1;
}
bool get_allfiles(string dir_name, vector<string>& files);
void print_bitmap(uint64_t uid, int cat_id);

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

    set<uint64_t> active_uids;
    // область для временных данных
    {
        // пробегаем по каталогу А, для каждого файла создаем множество ID
        // all_uids_A
        set<uint64_t> all_uids_A;
        for (auto&& f : files_A) {
            cerr << folder_A + "/" + f << endl;
            ifstream file(folder_A + "/" + f);
            if (file.is_open()) {
                string uid;
                while (getline(file, uid)) {
                    all_uids_A.insert(stoull(uid));
                }
                file.close();
            }
        }

        // пробегаем по каталогу B, для каждого файла создаем множество ID
        // all_uids_B
        set<uint64_t> all_uids_B;
        for (auto&& f : files_B) {
            cerr << folder_B + "/" + f << endl;
            ifstream file(folder_B + "/" + f);
            if (file.is_open()) {
                string uid;
                while (getline(file, uid)) {
                    all_uids_B.insert(stoull(uid));
                }
                file.close();
            }
        }

        cerr << "Пересекаем множества А и B" << endl;
        // Пересекаем all_uids_A с all_uids_B, строим active uids
        for (auto&& a : all_uids_A) {
            if (all_uids_B.find(a) != all_uids_B.end())
                active_uids.insert(a);
        }
        cerr << "Всего найдено " << active_uids.size() << " пересечений."
             << endl;

        // Очищаем память
        all_uids_A.clear();
        all_uids_B.clear();
    }

    // пробегаем по каталогу А, для каждого файла создаем свое множество id
    map<string, set<uint64_t>> uids_A;
    for (auto&& f : files_A) {
        cerr << folder_A + "/" + f << endl;
        ifstream file(folder_A + "/" + f);
        if (file.is_open()) {
            string uid_s;
            while (getline(file, uid_s)) {
                uint64_t uid = stoull(uid_s);
                // Пдоверяем встречался ли это тот uid в B вообще, активен ли
                if (active_uids.find(uid) != active_uids.end())
                    uids_A[f].insert(uid);
            }
            file.close();
        }
    }

    // пробегаем по каталогу B и для каждого встреченного id выставляем
    // бит присутствия
    map<uint64_t, uint64_t> bitmap;
    map<uint64_t, uint64_t> bitmap_all;

    uint64_t bit = 1;
    for (auto&& f : files_B) {
        cerr << folder_B + "/" + f << endl;
        ifstream file(folder_B + "/" + f);
        if (file.is_open()) {
            string uid_s;
            while (getline(file, uid_s)) {

                // Общая история уидов
                uint64_t uid = stoull(uid_s);
                bitmap_all[uid] |= bit;

                // Проверяем встречался ли этот uid в А, является ли ЦА
                if (active_uids.find(uid) != active_uids.end()) {
                    // ищем по категориям и выставляем бит у техт uid'ов,
                    // которые включены в оба множества, категории и даты
                    for (auto&& a : uids_A) {
                        if (a.second.find(uid) != a.second.end())
                            bitmap[uid] |= bit;
                    }
                }
            }
            file.close();
            bit <<= 1;
        }
    }

    // выводим хронологию актинвости
    // для каждой категории
    uint cat_id = 1;
    uint count = 0;
    for (auto&& a : uids_A) {
        // для каждого uida
        for (auto&& uid : a.second) {
            print_bitmap(bitmap[uid], cat_id);
            count++;
        }
        cat_id++;
    }
    cerr << "Активные пользователи: " << count << endl;

    // Выводим хронологию всех прочих
    uint count_0 = 0;
    cat_id = 0;
    count /= 4;
    for (auto&& uid : bitmap_all) {
        // Если uid из активной аудитории - пропускаем
        if(bitmap.find(uid.first)!=bitmap.end()) continue;
        cout << uid.first << " ";
        print_bitmap(uid.second, cat_id);
        // Выходим через 2х активных пользователей
        // if(++count_0 > count) break;
    }

    return 0;
}

void print_bitmap(uint64_t bitmap, int cat_id) {
    // id категории
    cout << cat_id << " ";
    // для каждого бита
    for (uint i = 0; i < 64; i++) {
        float avr_b = 0.0;
        int s = i> 0 ? -1 : 0;
        int e = i<63 ?  1 : 0;
        // count average bitcount
        uint64_t mask = bitmap >> (i+s);
        for(int b =s; b<=e; b++) {
            if (mask & 0x1) avr_b += 1.0;
            mask >>= 1;
        }
        avr_b /= e - s + 1.0;
        if(avr_b > 0.0)
            cout << i+1 << ":" << avr_b << " ";
    }
    cout << endl;
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
        sort(files.begin(), files.end());
        closedir(dir);
        return 1;
    }

    cerr << "Не могу открыть каталог " << dir_name;
    return 0;
}
