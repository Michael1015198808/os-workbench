static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

static inline int atoi8(const char* nptr){
    int ret=0;
    for(;*nptr;++nptr){
        ret*=8;
        ret+=*nptr-'0';
    };
    return ret;
}

static inline int atoi10(const char* nptr){
    int ret=0;
    for(;*nptr;++nptr){
        ret*=10;
        ret+=*nptr-'0';
    };
    return ret;
}

int atoi16(const char* nptr){
    int ret=0;
    for(;*nptr;++nptr){
        ret*=16;
        if(*nptr>='0'&&*nptr<='9'){
            ret+=*nptr-'0';
        }else if(*nptr>='a'&&*nptr<='z'){
            ret+=*nptr-'a'+10;
        }else if(*nptr>='A'&&*nptr<='Z'){
            ret+=*nptr-'A'+10;
        }
    };
    return ret;
}

int atoi(const char* nptr){
    if(nptr[0]=='0'){
        if(nptr[1]=='x'){
            return atoi16(nptr+2);
        }else{
            return atoi8(nptr+1);
        }
    }else{
        switch(nptr[0]){
            case '-':
                return -atoi10(nptr+1);
            case '+':
                ++nptr;
            default:
                return atoi10(nptr);
        }
    }
}
