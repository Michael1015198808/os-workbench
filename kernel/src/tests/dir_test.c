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
    {"/test/"   ,"test/"    ,"/test/test"},
    {"/test/"   ,"."        ,"/test"},
    {"/test/"   ,".."       ,"/"},
    {"/test/"   ,"a/../b"   ,"/test/b"},
    {"/test/"   ,"../.."    ,"/"},
};
void dir_test_init(void){
    char dest[0x100];
    for(int i=0;i<LEN(tests);++i){
        strcpy(dest,tests[i].pwd);
        dir_cat(dest,tests[i].rela);
        if(strcmp(dest,tests[i].ans)){
            Assert(0,"wrong on test %d\ndir_cat(%s,%s) should be %s\nyour ans:%s\n"
                    ,i
                    ,tests[i].pwd
                    ,tests[i].rela
                    ,tests[i].ans
                    ,dest);
            return;
        }
    }
    printf("dir_test PASSED!\n");
    while(1);
}
#endif
