#ifndef __FS_H
#define __FS_H
//Only specific filesystem should include this header file

static int is_dir(inode_t*);
#define check(inode,path) \
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
        if(!*path)return (inode_t*)inode; \
        return 0; \
    }while(0);

#endif//__FS_H
