#ifndef __DIR_H__
#define __DIR_H__

inline int get_first_slash_from(const char* const path,int idx);
inline int get_first_slash(const char* const path);
inline int get_last_slash_from (const char* const path,int idx);
inline int get_last_slash (const char* const path);
void dir_cat(char* dest,const char* src);

#endif//__DIR_H__
