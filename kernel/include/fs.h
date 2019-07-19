#ifndef __FS_H
#define __FS_H
//Only specific filesystem should include this header file

static int is_dir(inode_t*);

//check may call return, so I prefer write it as a macro
#define check(inode,path,flags) \
    do{ \
        if(!inode){ \
            warn("No such a file or directory"); \
            return NULL; \
        } \
        if(*path=='/'){ \
            while(*path=='/')++path; \
            if(!is_dir(inode)){ \
                warn("Not a directory"); \
                return NULL; \
            } \
        } \
        if(!*path){ \
            if((flags & O_DIRECTORY) && !is_dir(inode)){ \
                warn("Not a directory"); \
                return NULL; \
            } \
            else return (inode_t*)inode; \
        } \
    }while(0);

#endif//__FS_H
