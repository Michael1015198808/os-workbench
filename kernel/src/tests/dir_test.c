#include <devices.h>
#include <common.h>
#include <klib.h>
#include <dir.h>

#ifndef NO_TEST
typedef const char* dir;
struct{
    dir pwd,rela,ans;
}tests[]={
    {"/"        ,"test"     ,"/test"},
    {"/test"    ,"test"     ,"/test/test"},
    {"/test/"   ,"test"     ,"/test/test"},
    {"/test/"   ,"test/"    ,"/test/test"}
};
void dir_test_init(void){
    char dest[0x100];
    for(int i=0;i<LEN(tests);++i){
        strcpy(dest,tests[i].pwd);
        dir_cat(dest,tests[i].rela);
        if(strcmp(dest,tests[i].ans)){
            Assert(0,"dir_cat(%s,%s) should be %s\nyour ans:%s\n"
                    ,tests[i].pwd
                    ,tests[i].pela
                    ,tests[i].ans
                    ,dest);
            return;
        }
    }
}
#endif
