/*
structure inspired by: https://github.com/jmreyes/simple-c-shell/blob/master/simple-c-shell.c
			https://github.com/stpddream/OSHomework/tree/master/hw3
*/
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
//#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <pwd.h>
#include "parser.h"
#include "run_command.h"
#include "ll.h"

#define MAXLEN 256
#define MAXLINE 1024
#define NEGATIVE -1

// function prototypes.
void ourPrompt();
void initShell();
void sigchld_handler(int sig, siginfo_t *sif, void *notused);

// global variables
char** args_without_pipe;  //  xxx | xxx | xxx | ... 
char** args; // arguments in each *command
char** command; // command line that has been separated by ";"
char* line; // the whole command line
char* jobLine; // things we will store in job struct
sigset_t child_mask;

void sigchld_handler(int sig, siginfo_t *sif, void *notused) {
	//int status;
	// first get the process id of the child process and get the pgid of the child process
	pid_t pgid = getpgid(sif->si_pid);
	if(pgid == NEGATIVE) {
		perror("Get pgid error:");
		exit(EXIT_FAILURE);
	}
	// Then get the job with pgid;
	Job* job = getJobPid(pgid);
	if (job == NULL) {
		printf("getJobPid has error.");
	}
	//check whether the calling process is the one that has changed status, wait without blocking
	//if (sif->si_pid == waitpid(pgid, &status, WUNTRACED)) {
		//check what is the exit status of the calling process
		/* if the job normally exit, update its status, print the job status, remove it from job list
			 and then free the memory. Else if the job is suspended, update its status and print its status.
			 Else if the job is forced to terminate, update its status, print its status, remove it from the
			 job list, and then free the memory.
		*/
		if (WIFEXITED(sif->si_status)) 
		    {
					jobChangeStatus(job, JOBCOMP);
					printJobStatus(job);
		    	//if(jobRemovePid(pgid) != TRUE)
			//			printf("Job removal from the job list error.\n");
			//		freeJob(job); // free this job
		      return;
		    }
		    if (WIFSTOPPED(sif->si_status)) 
		    {
					jobChangeStatus(job, JOBSTOP);
					printJobStatus(job);
		      return;
		    }
		    if ( (WTERMSIG(sif->si_status) <= 12) || (WTERMSIG(sif->si_status) == 15))
		    {
					jobChangeStatus(job, JOBTERM);
					printJobStatus(job);
		    	//if(jobRemovePid(pgid) != TRUE)
			//			printf("Job removal from the job list error.\n");
			//		freeJob(job); // free this job
		      return;
		    }
		//}
		return;
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

void initShell() {
  myShTerminal = STDOUT_FILENO;
  if(!isatty(myShTerminal))
    perror("not a tty device");
  else {
        if (tcgetattr(myShTerminal, &myShTmodes) != 0)
           perror("tcgetattr error");
        else {
           if (myShTmodes.c_lflag.c_iflag & IXON)
               printf("Terminal start and stop is enabled\n");
           if (myShTmodes.c_lflag.c_lflag & ICANON)
               printf("Terminal is in canonical mode\n");
        }
     }
}

int main(int argc, char** argv) {
	//initShell();
	jobInit();
	int status = 0;
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
	sigaction(SIGCHLD, &sa, NULL);
	
	sigset_t block_mask;

    	sigaddset(&block_mask, SIGINT);
    	sigaddset(&block_mask, SIGTSTP);
    	sigaddset(&block_mask, SIGTERM);
    	sigaddset(&block_mask, SIGQUIT);
    	sigaddset(&block_mask, SIGTTOU);
    	sigaddset(&block_mask, SIGTTIN);
    	sigprocmask(SIG_BLOCK, &block_mask, NULL);
	sigemptyset(child_mask);
	sigaddset(&child_mask, SIGINT);
    	sigaddset(&child_mask, SIGTSTP);
    	sigaddset(&child_mask, SIGTERM);
    	sigaddset(&child_mask, SIGQUIT);
    	sigaddset(&child_mask, SIGTTOU);
    	sigaddset(&child_mask, SIGTTIN);
	sigaddset(&child_mask, SIGCHLD);


	int numCommands;
	int numArguments;
	int numSegments;
	while(1) {
		// print our shell prompt
		ourPrompt();
		command = (char**) malloc(MAXLINE * sizeof(char*));
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
			args_without_pipe = (char**) malloc(sizeof(char*) * MAXLEN);
			strcpy(jobLine, command[i]);
			numArguments = parseArguments(command[i], args_without_pipe);

			Process* toAdd = (Process*) malloc(sizeof(Process));
			toAdd->args = args_without_pipe;
			toAdd->argn = numArguments;
			toAdd->next = NULL;
			/*Process* temp = dummy;
			for(int j = 0; j < numSegments; j++) {
				args = (char**) malloc(sizeof(char*) * MAXLEN);
				Process* toAdd = (Process*) malloc(sizeof(Process));
				numArguments = parseArguments(argsWithPipe[j], args);
				toAdd->args = args;
				toAdd->argn = numArguments;
				toAdd->next = NULL;

				temp->next = toAdd;
				temp = toAdd;
			}*/
			int field;
			int status;
			if (numArguments < 0) {
				field = JOBBACK;
			}
			else {
				field = JOBFORE;
			}
			status = JOBRUN;
			Job* job = createJob(jobLine, toAdd, status, field);

			// I don't know what to do with status right now, will figure out in a moment 
			//free(dummy);// shouldn't free it here, process will be automatically freed when the job is removed from joblist
            if(jobInsert(job) == FALSE) {
            	printf("Add job fail!\n");
            	exit(1);
            }
            executing_command_without_pipe(job);
		}


	}
	return 0;

}
