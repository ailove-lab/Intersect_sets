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

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <array>
#include <algorithm>

#include <dirent.h>

#define MAX_BITS 64

using namespace std;

int usage();
bool get_allfiles(string dir_name, vector<string>& files);
//  interview   uid     activity
set<string>                       interviews_set;
map<string, set<string>>          interviews_map;
map<string, array<int, MAX_BITS>> interviews_stat;

int main(int argc, char** argv) {

  if(argc < 3) return usage();

  // get interview
  string path = argv[1]; // interview ids
  vector<string> files;
  get_allfiles(path, files);
  sort(files.begin(), files.end());
  for(auto&& f : files) cerr << f << endl;
  for(auto&& f : files) {
    string filename = path + "/" + f;
    ifstream file(filename);
    if (file.is_open()) {
      interviews_map[f]  = set<string>{};           // init map
      interviews_stat[f] = array<int, MAX_BITS>{0}; // init stat
      string uid;
      // iterate throught lines
      while (getline(file, uid)) {
        interviews_set.insert(uid);    // total uid list
        interviews_map[f].insert(uid); // per list uid list
        if(uid[uid.size()-1]=='\r') uid.resize(uid.size()-1);
      }
    } else {
      cerr << "can't open " << filename << endl;
    }
  }
  
  // get activity
  files.clear();
  path = argv[1]; // activity ids
  get_allfiles(path, files);
  sort(files.begin(), files.end());
  for(auto&& f : files) cerr << f << endl;
  int bit = 0;
  // iterate throught files
  for(auto&& f : files) {
    string filename = path + "/" + f;
    ifstream file(filename);
    if (file.is_open()) {
      string uid;
      // iterate throught lines
      while (getline(file, uid)) {
        if(uid[uid.size()-1]=='\r') uid.resize(uid.size()-1);
        // check if uid beloving interviewers
        if(interviews_set.find(uid) != interviews_set.end()) {
          // find interviewer at list
          for(auto&& i : interviews_map) {
            // if interview has uid, increment col
            if(i.second.find(uid) != i.second.end()) {
              interviews_stat[i.first][bit]++;
            }
          }
        }
      }
      bit++;
    } else {
      cerr << "cant't open " << filename << endl;
    }
  }

  // пробегаем по уидам, выводим
  for(auto&& s : interviews_stat) {
    cout << s.first << " ";
    for(auto&& a : s.second) {
      cout << a << " ";
    }
    cout << endl;
  }

  return 0;
}


// Получаем список файлов
bool get_allfiles(string dir_name, vector<string>& files) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dir_name.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if(ent->d_name[0] == '.') continue;
            files.push_back(ent->d_name);
        }
        closedir (dir);
        return 1;
    }

    cerr << "Не могу открыть каталог "<< dir_name;
    return 0;

}

// инструкция
int usage() {
    cout << "Usage:" << endl;
    cout << "\t./uids_activity interview_folder >> stat.txt" << endl;
    return 1;
}
