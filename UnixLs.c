

#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>


extern int errno;

const int BUFFER_SIZE = 512;
const int NUM_FILES_PER_LINE = 10;


void ls_func(char* dir, int op_L, int op_I, int op_R){//should dir be const?
    
    DIR *directory = opendir(dir); //opens the directory. Included in dirent.h
    if(!directory){//basic error checking
        if(errno == ENOENT){
            perror("Directory does not exist"); //Not found //print dir
        }else{
            perror("Directory is Unreadable"); //No read permission //***** should just print dir and not exit
        }
        return;//exit(EXIT_FAILURE);
    }
    struct dirent *mydir;
    struct stat mystat;
    char buf[BUFFER_SIZE];
    if(op_R){
        printf("%s:\n",dir);
    }

    // This loops run until an unreadable file is encountered 
    if(op_L){//long listing
    // This loops run until an unreadable file is encountered 
        while((mydir = readdir(directory)) != NULL){
            //Adapted from:stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c
            sprintf(buf, "%s/%s", dir, mydir->d_name);
            if(stat(buf, &mystat)==-1){
                perror("stat:");
            }
            if(mydir->d_name[0]== '.'){//always ignore hidden files
                continue;
            }
            if(op_I){
                printf("%5d ",(int)mystat.st_ino);
            }
            //check which mode bits are set using bitwise operation
            //Code adapted from https://stackoverflow.com/questions/10323060
            printf( (S_ISDIR(mystat.st_mode)) ? "d" : "-");// check for links?*************************************************************
            printf( (mystat.st_mode & S_IRUSR) ? "r" : "-");
            printf( (mystat.st_mode & S_IWUSR) ? "w" : "-");
            printf( (mystat.st_mode & S_IXUSR) ? "x" : "-");
            printf( (mystat.st_mode & S_IRGRP) ? "r" : "-");
            printf( (mystat.st_mode & S_IWGRP) ? "w" : "-");
            printf( (mystat.st_mode & S_IXGRP) ? "x" : "-");
            printf( (mystat.st_mode & S_IROTH) ? "r" : "-");
            printf( (mystat.st_mode & S_IWOTH) ? "w" : "-");
            printf( (mystat.st_mode & S_IXOTH) ? "x" : "-");
            printf(" %2d", (int)mystat.st_nlink);//# hard links
            //Need error checking for pw and grp to make sure >0
            struct passwd *pw = NULL;
            pw = getpwuid(mystat.st_uid);//user id
            printf(" %-8s", pw->pw_name);
            struct group *grp = NULL;
            grp = getgrgid(mystat.st_gid); //group id
            printf("%-8s", grp->gr_name);
            printf("%5d", (int)mystat.st_size); //size
            char buff[20];//extra buffer to be safe
            struct tm * timeinfo;
            timeinfo = localtime (&mystat.st_mtime);
            strftime(buff, sizeof(buff), " %b %d %H:%M ", timeinfo);//mmm dd yyyy hh:mm
            printf("%-8s",buff);
            printf("%-8s\n", mydir->d_name);
            //printf("\n");
        }
    }else{//not long listing
    int count = 0;
        while((mydir = readdir(directory)) != NULL){
            if(mydir->d_name[0]== '.'){//always ignore hidden files
                continue;
            }
            if(op_I){
                sprintf(buf, "%s/%s", dir, mydir->d_name);
                if(stat(buf, &mystat)==-1){
                    perror("stat:");
                }
                printf("%d  ",mystat.st_ino);
            }
            printf("%s  ", mydir->d_name);
            if(count == NUM_FILES_PER_LINE){
                printf("\n");
                count = 0;
            }else{
                count++;
            }
        }
        
        printf("\n");
    }    
    closedir(directory);
}


int main (int argc, char *argv[]) {
    if (argc == 1)
	{
		ls_func(".",0,0,0);
	}
	else if (argc == 2){
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
					printf("Incorrect flags\n");
					exit(EXIT_FAILURE);
				}
				p++;
			}
			ls_func(".",op_L,op_I,op_R);
		}else{//single argument is a path
            ls_func(argv[1],0,0,0);

        }
        
	} else if(argc == 3){ //flag and path
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
        //Handle printing "dir:" for -l here only if recursive is set to false*
        //need loop to iterate through command line input 
        //also probably need error checking on bad input
    } 
	return 0;
}
//TODO: double check casting for printf and properly format printing 
//better recovery for unreadable files 