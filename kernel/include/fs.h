#ifndef __FS_H
#define __FS_H
//Only specific filesystem should include this header file

static int is_dir(inode_t*);
static inline inode_t* check(inode_t* inode,const char** ori_path){
    if(!inode){
        warn("No such a file or directory");
        return NULL;
    }

    const char* path=*ori_path;
    if(*path=='/'){
        while(*path=='/')++path;
        if(!is_dir(inode)){
            warn("Not a directory");
            return NULL;
        }
    }
    if(!*path)return (inode_t*)inode;

    *ori_path=path;
}

#endif//__FS_H
