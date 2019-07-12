#ifndef __DIR_H__
#define __DIR_H__

int get_first_slash_from(const char* const path,int idx);
int get_first_slash(const char* const path);
int get_first_layer(const char* const path);
int get_last_slash_from (const char* const path,int idx);
int get_last_slash (const char* const path);
const char* get_pwd(void);
void dir_cat(char* dest,const char* src);
void to_absolute(char* dest,const char* pwd,const char* rela);

#endif//__DIR_H__
