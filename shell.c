#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include "parser.h"
#include "run_command.h"
#include "ll.h"

#define MAXLEN 256
#define MAXLINE 1024

void myPrompt();
void initShell();
char** argsWithPipe;  //  xxx | xxx | xxx | ... 
char** args; // arguments in each *command
char** command; // command line that has been separated by ";"
char* line; // the whole command line
char* jobLine; // things we will store in job struct
int myShTerminal;
pid_t myShPGid;
struct termios myShTmodes;


int main(int argc, char** argv) {
	initShell();
	jobInit();
	int numCommands;
	int numArguments;
	int numSegments;
	while(1) {
		command = (char**) malloc(MAXLINE * sizeof(char*));
		myPrompt();
		if((line = readline(">>>>>>>")) == NULL) {
			perror("IO error\n");
			free(command);
			freeJobList();
			exit(1);
		}
		numCommands = parseCommands(line, command);  // parse a command line into different
													// processes (by ";")

		for(int i = 0; i < numCommands; i++){
			jobLine = (char*) malloc(sizeof(char) * strlen(command[i]));
			argsWithPipe = (char**) malloc(sizeof(char*) * MAXLEN);
			strcpy(jobLine, command[i]);
			numSegments = parseSegments(command[i], argsWithPipe);

			Process* dummy = (Process*) malloc(sizeof(Process));
			Process* temp = dummy;
			for(int j = 0; j < numSegments; j++) {
				args = (char**) malloc(sizeof(char*) * MAXLEN);
				Process* toAdd = (Process*) malloc(sizeof(Process));
				numArguments = parseArguments(argsWithPipe[j], args);
				toAdd->args = args;
				toAdd->argn = numArguments;
				toAdd->next = NULL;

				temp->next = toAdd;
				temp = toAdd;
			}
			Job* job = createJob(jobLine, dummy->next, int status); 
			// I don't know what to do with status right now, will figure out in a moment 
			free(dummy);
			exeJ(job);
		}


	}
	return 0;

}

