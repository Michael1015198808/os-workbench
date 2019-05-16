//No lock, so can be called by gdb when dead-locked.
void _putc(char*);
void print(char* s){
    do{
    _putc(s);
    }while(++s);
}
