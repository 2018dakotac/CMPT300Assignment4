

#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

const int BUFFER_SIZE = 512;

//Adopted from:iq.opengenus.org/ls-command-in-c/
void ls_func(char* dir, int op_L, int op_I, int op_R){//should dir be const?
    printf("dir %s\n",dir);
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
    
    struct dirent *mydir;
    struct stat mystat;
    char buf[BUFFER_SIZE];
    // This loops run until an unreadable file is encountered
    while((mydir = readdir(directory)) != NULL){
        //memset(buf,'\0',BUFFER_SIZE);
        //Adapted from:stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c
        sprintf(buf, "%s/%s", dir[1], mydir->d_name);
        //printf("calling stat on %s",buf);
        stat(buf, &mystat);
        printf("%d", (int)mystat.st_size);//is this right cast?
        //For hidden files
        if(mydir->d_name[0]== '.'){//ignore hidden files
            continue;
        }
        printf("%s\n", mydir->d_name);
        
        if(op_L){
            printf("\n");
        }
    }
    if(!op_L){
        printf("\n");
    }
    
    closedir(directory);
}


int main (int argc, char *argv[]) {
    if (argc == 1)
	{
		ls_func(".",0,0,0);
	}
	else if (argc == 2)
	{
		if (((char*)argv[1])[0] == '-')//check if argument a flag
		{
			//Checking which flags have been passed
			int op_L = 0, op_I = 0,op_R = 0;
			char *p = (char*)(argv[1] + 1);
			while(*p){
				if(*p == 'l') op_L = 1;
				else if(*p == 'i') op_I = 1;
                else if( *p == 'R') op_R = 1;
				else{
					perror("Option not available");
					exit(EXIT_FAILURE);
				}
				p++;
			}
			ls_func(".",op_L,op_I,op_R);
		}else{//argument is a path
            ls_func(argv[1],0,0,0);

        }
        
	}
    else if(argc == 3){ //flag and path
        //Checking which flags have been passed
			int op_L = 0, op_I = 0,op_R = 0;
			char *p = (char*)(argv[1] + 1);
			while(*p){
				if(*p == 'l') op_L = 1;
				else if(*p == 'i') op_I = 1;
                else if( *p == 'R') op_R = 1;
				else{
					perror("Option not available");
					exit(EXIT_FAILURE);
				}
				p++;
			}
			ls_func(argv[2],op_L,op_I,op_R);
    } 
    else { //argc is >3 
        //need loop to handle multiple path and flag input
        //also need error checking on bad input
    } 
	return 0;
}
