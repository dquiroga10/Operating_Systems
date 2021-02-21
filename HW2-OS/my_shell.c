#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include  <sys/wait.h>
#include <errno.h>
#include "list.h"

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
extern int errno ;
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
	
	//char delim[] = " ";


	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	struct linked_list* list = ll_create();

	while(1) {			
		/* BEGIN: TAKING INPUT */
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
		int length = 0;
		int dAnd = 0;
		int tAnd = 0;
		int multiple = 0;
		int exit = 0;
		for(i=0;tokens[i]!=NULL;i++){
			length = i;
			if ( (strcmp(tokens[i], "&&") == 0)){
				dAnd =1;
				multiple =1;
			} else if(strcmp(tokens[i], "&&&") == 0){
				tAnd = 1;
				multiple =1;
			} else if(strcmp(tokens[i], "exit") == 0){
				exit = 1;
			}
		}

		char *args[64];
		int status;
		char path[] = "/bin/";
		pid_t pid;
		

		//TODO: make it wait in the foreground while parallel commands finish
		if (multiple == 1){
			char *Margs[64][64];
			int Length[64];
			int cc = 0;
			int ci = 0;
			int curr_length = ll_length(list);
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
			int pids[64];
			for (i=0;i <= cc;i++){
			 	int exec;
			 	int execution;
			 	char *argv[64];
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
			 	pid = fork();
			 	if(pid>0){
			 		ll_add(list, pid);
			 	}
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
			 		pids[i] = pid;
			 		if(dAnd == 1){
			 			waitpid(pid, &execution, 0);
			 			ll_remove_first(list);
			 		}
			 	}


			 	if(exec == -1){
			 		perror("Shell: Incorrect command");
			 	}

			 	for(int j = 0; j<64;j++) {
			 		argv[j] = NULL;
			 	}
			}

			//wait here for pids -- parallel
			int execution;
			while(ll_length(list)>curr_length){
				waitpid(ll_remove_first(list), &execution, 0);
			}
			while(ll_length(list)>0){
				printf("Shell: Background process finished\n");
				waitpid(ll_remove_first(list), &execution, 0);
			}

			
			// for (i=0;i <= cc;i++){
			// 	printf("%d\n", pids[i]);
			// 	waitpid(pids[i], &execution, 0);
			// }

			

			for (i=0;i <= cc;i++){
				for (ci=0; ci < Length[i];ci++){
					Margs[i][ci] = NULL;
				}
			}
		}
		else{

			//TODO: Kill all proceses that may be running and create exit command
			int execution;
			while(ll_length(list)>0){
				waitpid(ll_remove_first(list), &execution, 0);
				printf("Shell: Background process finished\n");
			}
			
			int and = 0;
			if(strcmp(tokens[length], "&") == 0){
				tokens[length] = NULL;
				and = 1;
			}

			pid = fork();
			if(pid>0){
		 		ll_add(list, pid);
		 	}

			if (pid == 0){
				//do whatever you want with the commands, here we just print them
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
					ll_remove_first(list);
					wait( &status );
				}
				
			}

			
		}
  		
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
			args[i] = NULL;
		}
		free(tokens);

		if (exit == 1){
			break;
		}
	}
	int execution;
	while(ll_length(list)>0){
		printf("Shell: Kill process finished\n");
		kill(ll_remove_first(list), SIGKILL);
	}
	ll_destroy(list);

	return 0;
}
