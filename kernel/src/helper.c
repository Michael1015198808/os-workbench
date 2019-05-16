//No lock, so can be called by gdb when dead-locked.
void _putc(char*);
void printf(char* s){
    do{
    _putc(s);
    }while(++s);
}
