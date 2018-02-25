/* Example of using sigaction() to setup a signal handler with 3 arguments
* including siginfo_t.
*/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

static void hdl (int sig, siginfo_t *siginfo, void *context)
{
	printf("Child terminated\n.");
}

int main (int argc, char *argv[])
{
	struct sigaction act;
	pid_t pid;
	int status;
	char* cmd = "ls\0";
	char** args = malloc(sizeof(char*)*2);
	args[0] = cmd;
	args[1] = NULL;

	memset (&act, '\0', sizeof(act));
	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &hdl;
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGCHLD, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

	pid = fork();
	if(pid == 0) {
		execvp(cmd, args);
	}
	else if (pid > 0) {
		waitpid(pid, &status, 0); //wait on the forked child...
	}
	else {
		printf("error occurred");
		exit(EXIT_FAILURE);
	}
	free(args);
	return 0;
