#include <devices.h>
#include <common.h>
#include <klib.h>
#include <dir.h>

#ifndef NO_TEST
struct{
    const char* src,dest,ans;
}const tests[]={
    {"/"        ,"test"     ,"/test"},
    {"/test"    ,"test"     ,"/test/test"},
    {"/test/"   ,"test"     ,"/test/test"},
    {"/test/"   ,"test/"    ,"/test/test"}
};
void dir_test_init(void){
    char dest[0x100];
    for(int i=0;i<LEN(tests);++i){
        strcpy(dest,tests[i].dest);
        dir_cat(dest,tests[i].src);
        if(strcmp(dest,tests[i])){
            Assert(0,"dir_cat(%s,%s) should be %s\nyour ans:%s\n"
                    ,tests[i].dest
                    ,tests[i].src
                    ,tests[i].ans
                    ,dest);
            return;
        }
    }
}
#endif
