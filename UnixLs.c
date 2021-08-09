#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>


extern int errno;

//const int BUFFER_SIZE = 512; 

const int NUM_FILES_PER_LINE = 10;//for non long listing
//NUM_FILES_PER_LINE will be divided by 2 if inode is printed

const int PATH_MAX = 512;



void ls_func(char* dir, int op_L, int op_I, int op_R){
    DIR *directory = opendir(dir); //opens the directory. Included in dirent.h
    if(!directory){//basic error checking
        if(errno == ENOENT){
            printf("%s\n",dir);
            perror("UnixLs"); //Not found
        }else{
            printf("%s\n",dir);
            perror("UnixLs"); //No read permission 
        }
        return;//exit(EXIT_FAILURE);
    }
    struct dirent *mydir;
    struct stat mystat;
    char buf[PATH_MAX];
    if(op_R){
        printf("%s:\n", dir);
    }
    // This loop runs until an unreadable file is encountered 
    if(op_L){//long listing
    // This loop runs until an unreadable file is encountered 
        while((mydir = readdir(directory)) != NULL){
            sprintf(buf, "%s/%s", dir, mydir->d_name);
            if(lstat(buf, &mystat)==-1){
                perror("lstat()");
                return;
            }
            if(mydir->d_name[0]== '.'){//always ignore hidden files
                continue;
            }
            if(op_I){
                printf("%5d ",(int)mystat.st_ino);
            }
            //check which mode bits are set using bitwise operation
            //Permission printing code adapted from https://stackoverflow.com/questions/10323060
            if(S_ISLNK(mystat.st_mode)){
                printf("l");
            }else{
            printf( (S_ISDIR(mystat.st_mode)) ? "d" : "-");
            }
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
            printf("%-8s", mydir->d_name);
            if(S_ISLNK(mystat.st_mode)){//check if link
                int bufferSize = mystat.st_size+1; //+1 for null char
                if(mystat.st_size == 0){//handle special symlink with size 0
                    bufferSize = PATH_MAX;
                }
                char * linkbuf = malloc(bufferSize);
                if (linkbuf == NULL){
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }
                memset(linkbuf,'\0',bufferSize);
                size_t nbytes = readlink(buf,linkbuf,bufferSize-1);
                if((int)nbytes == -1){
                    perror("readlink");
                    //exit(EXIT_FAILURE);
                }else{
                    //linkbuf[bufferSize-1] = '\0'; may as well play it safe with memset
                    printf(" -> %s",linkbuf);
                }
                free(linkbuf);
            }
            printf("\n");
        }
    }else{//not long listing
    int count = 0;
        while((mydir = readdir(directory)) != NULL){
            if(mydir->d_name[0]== '.'){//always ignore hidden files
                continue;
            }
            if(op_I){
                sprintf(buf, "%s/%s", dir, mydir->d_name);
                if(lstat(buf, &mystat)==-1){
                    perror("lstat()");
                    return;
                }
                printf("%-6d ",(int)mystat.st_ino);
            }
            printf("%-10s ", mydir->d_name);
            if(op_I && count == NUM_FILES_PER_LINE/2){
                printf("\n");
                count = 0;
            }
            if(count == NUM_FILES_PER_LINE){
                printf("\n");
                count = 0;
            }else{
                count++;
            }
        }
        printf("\n");
    }
    printf("\n");
    if(op_R){
        rewinddir(directory);// rewind directory stream
         while((mydir = readdir(directory)) != NULL){//iterate through looking for directories
            if (mydir->d_name[0] == '.'){//ignore hiddin files 
                continue;
            }
            sprintf(buf, "%s/%s", dir, mydir->d_name);
            if(lstat(buf, &mystat)==-1){
                perror("lstat()");
                return;
            }      
            if(S_ISDIR(mystat.st_mode)){//check if directory
                ls_func(buf, op_L, op_I, op_R);
            }
        }
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
    
    else{
        int i=2;
        int op_L = 0, op_I = 0,op_R = 0;
        char *p = (char*)(argv[1] + 1); //second argument will always be a flag or permutation of flags
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
        while(i < (argc-1)){
            if(((char*)argv[i])[0] == '-'){
            //check if argument is a flag or directory
                char *a = (char*)(argv[i] + 1);
			    while(*a){
				    if(*a == 'l') op_L = 1;
				    else if(*a == 'i') op_I = 1;
                    else if(*a == 'R') op_R = 1;
				    else{
					    printf("Incorrect flags\n");
					    exit(EXIT_FAILURE);
				    }
				    a++;
			    }

            }else{
                ls_func(argv[i],op_L,op_I,op_R);
            }
            i++;

        }
        if(((char*)argv[(argc-1)])[0] == '-'){
            //if last argument is not a directory then send current directory as argument
            char *a = (char*)(argv[2] + 1);
			while(*a){
				if(*a == 'l') op_L = 1;
				else if(*a == 'i') op_I = 1;
                else if(*a == 'R') op_R = 1;
				else{
					printf("Incorrect flags\n");
					exit(EXIT_FAILURE);
				}
				a++;
                ls_func(".",op_L,op_I,op_R);
			}

        }else{
            ls_func(argv[(argc-1)],op_L,op_I,op_R);
        }

    }
	return 0;
}

