

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

//Adopted from:iq.opengenus.org/ls-command-in-c/
void ls_func(char* dir, int op_L, int op_I, int op_R){//should dir be const?
    
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
    printf("%s:\n",dir);//should only print this if multiple paths passed
    // This loops run until an unreadable file is encountered
    while((mydir = readdir(directory)) != NULL){
        //memset(buf,'\0',BUFFER_SIZE);
        //Adapted from:stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c
        sprintf(buf, "%s/%s", dir, mydir->d_name);
        if(stat(buf, &mystat)==-1){
            perror("stat:");
        }
        if(mydir->d_name[0]== '.'){//always ignore hidden files
            continue;
        }
        //check which mode bits are set using bitwise operation
        ///https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
        printf( (S_ISDIR(mystat.st_mode)) ? "d" : "-");//*** check for links?
        printf( (mystat.st_mode & S_IRUSR) ? "r" : "-");
        printf( (mystat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (mystat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (mystat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (mystat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (mystat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (mystat.st_mode & S_IROTH) ? "r" : "-");
        printf( (mystat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (mystat.st_mode & S_IXOTH) ? "x" : "-");
        printf(" %d", (int)mystat.st_nlink);//# hard links
        //Need error checking for pw and grp to make sure >0
        struct passwd *pw = NULL;
        pw = getpwuid(mystat.st_uid);//user id
        printf(" %s", pw->pw_name);
        struct group *grp = NULL;
        grp = getgrgid(mystat.st_gid); //group id
        printf(" %s", grp->gr_name);
        printf(" %d", (int)mystat.st_size); //size
        //mmm dd yyyy hh:mm
        char buff[20];
        struct tm * timeinfo;
        timeinfo = localtime (&mystat.st_mtime);
        strftime(buff, sizeof(buff), "%b %d %H:%M", timeinfo);
        printf(" %s",buff);
        if(op_I){
            printf("Inode: %d ",(int)mystat.st_ino);
        }
        printf(" %s\n", mydir->d_name);
        
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
//TODO: double check casting for printf and properly format printing 