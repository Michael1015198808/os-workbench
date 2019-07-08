#ifndef __DIR_H__
#define __DIR_H__

int get_first_slash_from(const char* const path,int idx);
int get_first_slash(const char* const path);
int get_last_slash_from (const char* const path,int idx);
int get_last_slash (const char* const path);
void dir_cat(char* dest,const char* src);

#endif//__DIR_H__
