#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "list.h"
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
extern int errno ;



//Daniel Quiroga
//February 22, 2021
//Homework 2 - Operating Systems

struct linked_list* list;
int sig;

//This is our signal catcher for when Ctrl+C is clicked
void controlC(int sign_num){
	signal(SIGINT, controlC);
	sig = 1;
	//change our global sig variable and kill any process currently in the foreground
	if(ll_length(list)>0){
		kill(ll_remove_first(list), SIGKILL);
	}
}
/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;
	signal(SIGINT,controlC);
	
	//char delim[] = " ";


	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	//here we create our list that will keep track of every PID
	list = ll_create();

	while(1) {			
		/* BEGIN: TAKING INPUT */
		sig= 0;
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		//these flag variable will tell the program which condition the current commands fall under 
		int length = 0;
		int dAnd = 0;
		int multiple = 0;
		int exit = 0;

		//checking to see if we have an exit command or multiple commands separate by double or triple &
		for(i=0;tokens[i]!=NULL;i++){
			length = i;
			if ( (strcmp(tokens[i], "&&") == 0)){
				dAnd =1;
				multiple =1;
			} else if(strcmp(tokens[i], "&&&") == 0){
				multiple =1;
			} else if(strcmp(tokens[i], "exit") == 0){
				exit = 1;
			}
		}

		//initializing our array for command and path variable to call executables
		char *args[64];
		int status;
		char path[] = "/bin/";
		pid_t pid;
		
		//two separate section one for multiple commands and the other for single commands
		if (multiple == 1){
			char *Margs[64][64];
			int Length[64];
			int cc = 0;
			int ci = 0;
			int curr_length = ll_length(list);
			//we place all commands in its own index of Margs such that the first index refers to command number and the second is the token in that command
			for(i=0;tokens[i]!=NULL;i++){
				if ( (strcmp(tokens[i], "&&") == 0) || (strcmp(tokens[i], "&&&") == 0)){
					Length[cc] = ci;
					cc++;
					ci = 0;
				}
				else if(ci == 0){
					Margs[cc][ci++] = tokens[i];

				}else{
					Margs[cc][ci++] = tokens[i];
				}
			}
			Length[cc] = ci;

			//here we iterate through each command that was inputted
			for (i=0;i <= cc;i++){
				printf("sig: %d\n",sig );
				if(sig == 0){
				 	int exec;
				 	int execution;
				 	char *argv[64];
				 	//we go through each command in Margs and place in an array that will be easier for execution lower
				 	for(int j = 0; j<Length[i];j++) {
				 		if (j==0){
				 			strcpy(path, "/bin/");
				 			strcat(path, Margs[i][j]);
				 			argv[j] = path;
				 		}
				 		else{
				 			argv[j] = Margs[i][j];	
				 		}
				 	}

				 	//create child process and add that pid to the list of pids currently active
				 	pid = fork();
				 	if(pid>0){
				 		ll_add(list, pid);
				 	}

				 	//here we will execute each command and depending on the
				 	//flag we will do serial or parallel
				 	if(pid==0){
				 		if (strcmp(argv[0],"/bin/cd") == 0){
							
							exec = chdir(argv[1]);

						}else if (strcmp(argv[0],"/bin/exit") == 0){
							exit = 1;
						}
						else{
							exec = execv(argv[0], argv);
						}
				 	}
				 	else{
				 		if(dAnd == 1){
				 			waitpid(pid, &execution, 0);
				 			if(ll_length(list)>0){
				 				ll_remove_first(list);
				 			}		
				 		}
				 	}


				 	if(exec == -1){
				 		perror("Shell: Incorrect command");
				 	}

				 	//here we free up the space used to run the command so tha the next iterating as a null terminating array
				 	for(int j = 0; j<64;j++) {
				 		argv[j] = NULL;
				 	}
				}
			}

			//wait here for pids -- parallel
			//we will not ask for user input until all parallel processes are finished
			int execution;
			while(ll_length(list)>curr_length){
				waitpid(ll_remove_first(list), &execution, 0);
			}
			while(ll_length(list)>0){
				printf("Shell: Background process finished\n");
				waitpid(ll_remove_first(list), &execution, 0);
			}			


			//free up the space taken up by Margs and convert it back to NULL
			for (i=0;i <= cc;i++){
				for (ci=0; ci < Length[i];ci++){
					Margs[i][ci] = NULL;
				}
			}
		}
		else{

			//check to see if we have an background processes taht finished or need to finish and go ahead and finish them
			int execution;
			while(ll_length(list)>0){
				waitpid(ll_remove_first(list), &execution, 0);
				printf("Shell: Background process finished\n");
			}
			
			//checking to see if we have a background command
			int and = 0;
			if(strcmp(tokens[length], "&") == 0){
				tokens[length] = NULL;
				and = 1;
			}


			//create new child process and add it to list of pids
			pid = fork();
			if(pid>0){
		 		ll_add(list, pid);
		 	}

			if (pid == 0){
				//for the command we place in a NULL terminated array in order to execute
				for(i=0;tokens[i]!=NULL;i++){

					if(i == 0){
						strcat(path, tokens[i]);
						args[0] = path;
					}
					else{
						args[i] = tokens[i];
					}
				}
				int exec;

				//special case for cd and exit since these are not built in commands
				if (strcmp(args[0],"/bin/cd") == 0){
					exec = chdir(tokens[1]);
					
				}else if (strcmp(args[0],"/bin/exit") == 0){
					exit = 1;
					exec = 0;
				}
				else{
					exec = execv(args[0], args);
					
				}
				
				if(exec == -1){
					perror("Shell: Incorrect command");		
				}
				
			}
			else{
				if (and == 0){
					wait( &status );
					ll_remove_first(list);
				}
				
			}

			
		}
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
			args[i] = NULL;
		}
		free(tokens);

		//we will break here if exit is seen in the tokens
		if (exit == 1){
			break;
		}
	}
	int execution;
	//go and kill any process that is still in the list of pids to free up memory
	while(ll_length(list)>0){
		printf("Shell: Kill process finished\n");
		kill(ll_remove_first(list), SIGKILL);
	}
	//destroy the list used for pids
	ll_destroy(list);

	return 0;
}
