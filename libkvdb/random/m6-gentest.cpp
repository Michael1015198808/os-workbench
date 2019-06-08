#include <iostream>
#include <string>
#include <map>
#include <cstdio>
using namespace std;

string keys[16] = {
    "AAA", "BBB", "CCC", "DDD",
    "EEE", "FFF", "GGG", "HHH",
    "III", "JJJ", "KKK", "LLL",
    "MMM", "NNN", "OOO", "PPP"
};

string values[256] = { };

map<string, string> m;

int visited[16];

int gen_kvdb_put(){
    int idx1 = rand() % 16, idx2 = rand() % 256;
    visited[idx1] = 1;
    m[keys[idx1]] = values[idx2];
    //cerr << "put " << keys[idx1] << " " << values[idx2] << endl;
    cout << "kvdb_put(db, \"" << keys[idx1] << "\", \""
     << values[idx2] << "\");\n";
}

int gen_kvdb_get(){
    int bias = rand() % 16;
    for(int i = 0; i < 16; i ++) {
        if(visited[i + bias % 16]) {
            visited[i + bias % 16] = 0;
            //cerr << "get " << keys[i + bias] << ":"<< m[keys[i + bias]] << endl;
            //m.erase(keys[i + bias]);
            cerr << m[keys[i + bias]] << endl;
            m.erase(keys[i + bias]);
            cout << "printf(\"%s\\n\", (mystr = kvdb_get(db, \"" << keys[i + bias]
             << "\")) == NULL ? \"(null)\" : mystr);\n";
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    cout <<"#include <stdio.h>" <<endl;
    cout <<"#include <stdlib.h>" <<endl;
    cout <<"#include \"kvdb.h\"" <<endl;
    cout <<"int main(){" <<endl;
    cout <<"kvdb_t *db=malloc(sizeof(kvdb_t));" <<endl;
    cout <<"kvdb_open(db,DB_FILE);" <<endl;
    void test(int,char*[]);
    test(argc,argv);
    cout <<"kvdb_close(db);" <<endl;
    cout<<"return 0;" <<endl;
    cout<<"}";
    return 0;
}
void test(int argc,char *argv[]){
    srand(time(NULL));
    int seed = rand();

    if(argc == 2)
        sscanf(argv[1], "%d", &seed);
    srand(seed);
    cout << "/* srand seed: " << seed << "*/" << endl;
    cout << "char* mystr;" << endl;

    for(int i = 0; i < 256; i++) {
        int len = rand() % 31 + 1;
        for(int j = 0; j < len; j++)
            values[i] += rand() % 26 + 'a';  
    }

    for(int i = 0; i < 128; i++) {
        if(rand() % 3 || gen_kvdb_get() == 0) {
            gen_kvdb_put();
        }
    }

}
