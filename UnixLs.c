

#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

void ls_func(const char *dir, int a, int b){
    
    DIR *directory = opendir(dir); //opens the directory. Included in dirent.h
    if(!directory){
        if(errno == ENOENT){
            perror("This Directory does not exist"); //Not found
        }
        
        else{
            perror("This Directory is Unreadable"); //No read permission
        }
        
        exit(EXIT_FAILURE);
    }
    
    struct dirent *readdir(DIR *dirp);
    
}


int main () {
    
    printf("hello world!\n");

    INFODEMO();


    //TODO deal with symbolic hard/soft links last? 
    return 0;
}
