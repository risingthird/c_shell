/*Linyi Chen & Ziting Shen
Sample code
Reference: https://www.andrew.cmu.edu/course/15-310/applications/homework/homework4/signal_example2.html*/
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#define NEG -1


//this function is the signal handler 
void handle_sigchld(int sig, siginfo_t *sip, void *notused) {
  int status;
  //check whether the calling process is the one that has changed status, wait without blocking
  if (sip->si_pid == waitpid(sip->si_pid,&status, WNOHANG)) {
  	//check what is the exit status of the calling process
    if (WIFEXITED(status)) 
        {
          printf ("Voluntary exit.\n");
          goto done;
        }
        if (WIFSTOPPED(status)) 
        {
          printf ("Suspended.\n");
          goto done;
        }
        if ( (WTERMSIG(status) <= 12) || (WTERMSIG(status) == 15))
        {
          printf ("Croaked");
          goto done;
        }
        printf ("Nothing interesting\n");
		done:;
    }
  else{
  	//if no update, print not interested.
  	printf("Not interested\n");
  }
}

int main(){
	//declare the sample sigaction struct
	struct sigaction sa;
	//set the handler, in this case our handler has three arguments, so will set sa_sigaction.
	//instead of sa_handler
	sa.sa_sigaction = &handle_sigchld;
	//initialize an empty set for signals to be blocked.
	sigemptyset(&sa.sa_mask);
	//set flags, SA_SIGINFO flag is to specify signal handler in sa is sa_sigaction.
	sa.sa_flags = SA_SIGINFO;
	//register the signal SIGCHLD.
	sigaction (SIGCHLD, &sa, NULL);
	int suc = fork();
	//check whether fork is successful.
	if (suc == -1){
		printf("Fork failure!\n");
		exit(EXIT_FAILURE);
	}
	if (suc == 0){
		//if in child process, exit with success.
		exit(EXIT_SUCCESS);
	}
	else{
		int i;
		// let the parent process live for five seconds, and see whether there are updates from child process at each second
		for (i = 0; i < 5; i++){
			printf("%d second(s) have passed, let's see whether there is update from child.\n", i);
			//check whether sigaction is successful
			if (sigaction(SIGCHLD, &sa, NULL) == NEG) {
  				exit(EXIT_FAILURE);
			}
			sleep(1);
		}
	}
	exit(EXIT_SUCCESS);
}
