// Iterate over all files (id lists)
// build matrix of id inclusion at every list
// id1 -> list1, list2, ... , listN
// id2 -> list1, list2, ... , listN
// ...
// idM -> list1, list2, ... , listN


#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <bitset>
#include <algorithm>

#include <dirent.h>

#define MAX_BITS 64

using namespace std;

int usage();
bool get_allfiles(string dir_name, vector<string>& files);

// Пересечение двух групп множеств ID
int main(int argc, char** argv) {

  if(argc < 2) return usage();

  string path = argv[1];
  vector<string> files;

  get_allfiles(path, files);
  int files_count = files.size();
  sort(files.begin(), files.end());
  // print names
  for(auto&& f : files) {
    cout << f << endl;
  }
  
  map<string, bitset<MAX_BITS>> uids;
  int bit = 0;
  // iterate throught files
  for(auto&& f : files) {
    string filename = path + f;
    ifstream file(filename);
    if (file.is_open()) {
      string uid;
      // iterate throught lines
      while (getline(file, uid)) {
        if(uid[uid.size()-1]=='\r') uid.resize(uid.size()-1);
        uids[uid][bit] = 1;
      }
      bit++;
    } else {
      cerr << "can't open " << filename << endl;
    }
  }

  // пробегаем по уидам, выводим
  for(auto&& u : uids) {
    cout << u.first << " " << u.second << endl;
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
    cout << "\t./stat folder>> stat.txt" << endl;
    return 1;
}
