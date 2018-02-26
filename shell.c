/*
structure inspired by: https://github.com/jmreyes/simple-c-shell/blob/master/simple-c-shell.c
			https://github.com/stpddream/OSHomework/tree/master/hw3
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <pwd.h>
#include "parser.h"
#include "run_command.h"
#include "ll.h"

#define MAXLEN 256
#define MAXLINE 1024

// function prototypes.
void ourPrompt();
void initShell();
void sigchld_handler(int sig, siginfo_t *sif, void *notused);

// global variables
char** argsWithPipe;  //  xxx | xxx | xxx | ... 
char** args; // arguments in each *command
char** command; // command line that has been separated by ";"
char* line; // the whole command line
char* jobLine; // things we will store in job struct
int myShTerminal;
pid_t myShPGid;
struct termios myShTmodes;

void sigchld_handler(int sig, siginfo_t *sif, void *notused) {
	int status;
	// first get the process id of the child process and get the pgid of the child process
	pid_t pgid = getpgid(sif->si_pid);
	// Then get the job with pgid;
	Job* job = getJobPid(pgid);
	//check whether the calling process is the one that has changed status, wait without blocking
	if (sif->si_pid == waitpid(pid, &status, WNOHANG | WUNTRACED)) {
		//check what is the exit status of the calling process
		if (WIFEXITED(status)) 
		    {
		    	if(jobRemovePid(pid) != 1) {
						printf("Job removal from the job list error.\n");
					}
					
		      printf ("Voluntary exit.\n");
		      return;
		    }
		    if (WIFSTOPPED(status)) 
		    {
		      printf ("Suspended.\n");
		      return;
		    }
		    if ( (WTERMSIG(status) <= 12) || (WTERMSIG(status) == 15))
		    {
		      printf ("Croaked");
		      return;
		    }
		    printf ("Nothing interesting\n");
			return;
		}
	else{
		//if no update, print not interested.
		printf("Not interested\n");
	}
}

// print our shell prompt
void ourPrompt() {
  struct passwd *pwd; // using struct passwd from pwd.h
  char host[MAXLEN];
  char path[MAXLINE];
  pwd = getpwuid(getuid());  // get information about the user.
  getcwd(path, MAXLINE); // get the current working directory
  char *username = pwd->pw_name; // username
  char *homedirectory = pwd->pw_dir; // home directory
  
  if (gethostname(host, MAXLEN) == 0) {
    printf("[用心写bug，用脚写shell, only bugs live forever ┻━┻ ︵ヽ(`Д´)ﾉ︵﻿ ┻━┻, ¯\\_(ツ)_/¯ whatevs Broken Shell]\n[%s@%s ", username, host);
  } else
    printf("[用心写bug，用脚写shell, only bugs live forever ┻━┻ ︵ヽ(`Д´)ﾉ︵﻿ ┻━┻, ¯\\_(ツ)_/¯ whatevs Broken Shell]\n[%s@unknown ", username);

  if (strlen(path) < strlen(homedirectory) || strncmp(path, homedirectory, strlen(homedirectory)))
    printf("%s]", path);
  else
    printf("~%s]",path);

  // root: /  guest/user: $
  if (geteuid() == 0)
    printf("/ ");
  else
    printf("$ ");

  return;
}

int main(int argc, char** argv) {
	initShell();
	jobInit();
	
	/* Sigchild signal handling*/
	//declare the sample sigaction struct
	struct sigaction sa;
	//set the handler, in this case our handler has three arguments, so will set sa_sigaction instead of sa_handler.
	sa.sa_sigaction = &sigchld_handler;
	//initialize an empty set for signals to be blocked.
	sigemptyset(&sa.sa_mask);
	//set flags, SA_SIGINFO flag is to specify signal handler in sa is sa_sigaction.
	sa.sa_flags = SA_SIGINFO;
	//register the signal SIGCHLD.
	sigaction (SIGCHLD, &sa, NULL);

	int numCommands;
	int numArguments;
	int numSegments;
	while(1) {
		// print our shell prompt
		ourPrompt();
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
