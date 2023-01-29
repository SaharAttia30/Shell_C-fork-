#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

//int flag1 = 0 ;
void handler(){
	puts("\nYou typed Control-C!\n");
	//flag1 = 1 ;
}


int main() {
	char command[1024];
	char *token;
	char *outfile;
	int i, fd, amper, redirect, dont_exist, retid, status,changedir;
	char *argv[10];
	char *temp;
   	char* last_command[10];
	char prmpt[256] = "hello";
	signal(SIGINT, handler);
    while (1){
    
        int do_again = 0;
        printf("%s: ",prmpt);
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';
        /* parse command line */
        token = strtok (command," ");
        i = 0;
        if(token != NULL && !(strcmp(token,"!!") == 0)){
            do_again = 1;
        }
        else{
        	do_again = 0;
        }
        if (do_again == 1){
            while (token != NULL)
            {
                argv[i] = token;
                last_command[i] = strdup(argv[i]);
                token = strtok (NULL, " ");
                i++;
            }
            argv[i] = NULL;
            last_command[i] = NULL;
        }
        else{
            while (token != NULL)
            {
                argv[i] = token;
                token = strtok (NULL, " ");
                i++;
            }
            argv[i] = NULL;
        }
        /* Is command empty */
        if (argv[0] == NULL)
            continue;

        /* Does command line end with & */
        
        if (! strcmp(argv[i - 1], "&")) {
            amper = 1;
            argv[i - 1] = NULL;
        }
        else 
            amper = 0; 

        if (! strcmp(argv[i - 2], ">") || strcmp(argv[i - 2], "2>") == 0) {
           if(strcmp(argv[i - 2], "2>") == 0){
          	dont_exist = 1;
           }
           else{
            	dont_exist = 0;
            }
            redirect = 1;
            argv[i - 2] = NULL;
            outfile = argv[i - 1];
       }
        else 
            redirect = 0;
        /* for commands not part of the shell command language  */ 
       
     
        if(i > 2 && strcmp(argv[0], "prompt") == 0) {    			
	        strcpy(prmpt,argv[2]);
		 continue;
        }
      
        if(strcmp(argv[0], "quit") == 0) {
        	exit(0);
        }
        
        if(strcmp(argv[0], "echo") == 0) {
            if(strcmp(argv[1], "$?") == 0){
                printf("%d\n" ,status);
                continue;
            }
        }
        
        if(strcmp(argv[0], "cd") == 0){
        	if (opendir(argv[1]) == NULL) {
			printf("can't open %s \n", argv[1]);
			continue;
		}
	        chdir(argv[1]);
            	continue;
        } 
        
        
        if (fork() == 0) { 
            /* redirection of IO ? */
            if (redirect) {
                fd = creat(outfile, 0660);
                if(dont_exist == 1){
                	 close (STDERR_FILENO);
                }
                else{
                	close(STDOUT_FILENO);
                }
                dup(fd); 
                close(fd); 
                /* stdout is now redirected */
            } 
            if(do_again == 1){
            	execvp(argv[0], argv);
            }
            else if (do_again == 0){
            	execvp(last_command[0], last_command);
            }
        }      
        /* parent continues here */
        if (amper == 0)
            retid = wait(&status);
            
     }
}
