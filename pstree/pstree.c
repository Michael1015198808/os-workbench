#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
void printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 ||
                strcmp("..",entry->d_name) == 0)
                continue;
            printf("%*s%s/\n",depth,"",entry->d_name);
            /* Recurse at a new indent level */
            printdir(entry->d_name,depth+4);
        }
        else printf("%*s%s\n",depth,"",entry->d_name);
    }
    chdir("..");
    closedir(dp);
}

int main()
{
    printf("Directory scan of /proc:\n");
    printdir("/proc",0);
    printf("done.\n");
    exit(0);
}
//Copy from https://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
/*int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("%d\n",getpid());
  system("ls /proc");
  getchar();//Give me time to find it in /proc
  assert(!argv[argc]); // always true
  return 0;
}*/
