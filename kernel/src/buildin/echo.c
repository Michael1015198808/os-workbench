#include <klib.h>
int echo(void *args[]){
    int i=0;
    while(1){
        printf("%s",(char*)(args[i]));
        ++i;
        if(args[i]){
            printf(" ");
        }else{
            printf("\n");
            return 0;
        }
    }
}
