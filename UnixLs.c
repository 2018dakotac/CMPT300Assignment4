

#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

//Adopted from:iq.opengenus.org/ls-command-in-c/
void ls_func(const char *dir[], int op_a, int op_b){
    
    DIR *directory = opendir(dir[1]); //opens the directory. Included in dirent.h
    if(!directory){
        if(errno == ENOENT){
            perror("This Directory does not exist"); //Not found
        }
        
        else{
            perror("This Directory is Unreadable"); //No read permission
        }
        
        exit(EXIT_FAILURE);
    }
    
    struct dirent *mydir;
    struct stat mystat;
    char buf[512];
    // This loops run until an unreadable file is encountered
    while((mydir = readdir(directory)) != NULL){

        //Adapted from:stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c
        sprintf(buf, "%s/%s", dir[1], mydir->d_name);
        stat(buf, &mystat);
        printf("%d", mystat.st_size);
        //For hidden files
        if(!op_a && mydir->d_name[0]== '.'){
            continue;
        }
        printf("%s\n", mydir->d_name);
        
        if(op_b){
            printf("\n");
        }



    }
    if(!op_b){
        printf("\n");
    }
    
    closedir(directory);
}


int main () {
    
    printf("hello world!\n");

    INFODEMO();


    //TODO deal with symbolic hard/soft links last? 
    return 0;
}
