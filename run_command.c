#include "run_command.h"

#define WAIT_ANY -1
#define FAIL -1
#define FALSE 0
#define TRUE 1
//need to implement joblock() and jobunlock() to protect joblist
//void jobunlock();
//void joblock();

//prototype
void bFg(char** args, int argn);
void put_job_in_foreground(Job* job);

//global variable, Probably need to use extern
int myShTerminal;
pid_t myShPGid;
struct termios myShTmodes;

// built-in command: jobs
void bJobs() {
	printList();
	return;
}

void bKill(char** args, int argn) {
	int kill_flag = FALSE; //kill_flag is true when input has -9
	int dash_flag = 0;
	int percent_flag = 0;
	int is_jid = FALSE;
	int id;
	Job* job;
	int to_be_killed = 0;
	if(argn == 1) {
		printf("kill: usage: kill (signal) %jid (or pid).Currently, signal only support -9, SIGKILL.\n");
		return;
	}

	//parse the arguments and send proper respond
	for(int i = 1; i < argn; i++) {
		//if the input is invalid, we will print usage of kill and return
		if(dash_flag > 1 || percent_flag > 1) {
			rintf("kill: usage: kill (signal) \%jid (or pid).Currently, signal only support -9, SIGKILL.\n");
			return;
		}
		if(args[i][0] == '-'){
			dash_flag ++;
			if(strcmp(args[i], "-9") == 0) {
				kill_flag = TRUE; //probably we need to test whether kill_flag is false
			}
			else {
				printf("kill: usage: kill (signal) \%jid (or pid).Currently, signal only support -9, SIGKILL.\n");
				return;
			}
		}
		else if(args[i][0] == '%') {
			percent_flag ++;
			is_jid = TRUE;
			if(is_jid) {
				sscanf(args[i],"\%%d", &id);
			}
			else {
				if((id = atoi(args[i])) = 0) {
					printf("kill: usage: kill (signal) %jid (or pid).Currently, signal only support -9, SIGKILL.\n");
					return;
				}
			}
		}
	}

	//find the actual job
	if(is_jid) {
		job = getJobJobId(id);
	}
	else
		job = getJobPid(id);
	if(job == NULL) {
		printf("invalid job number or process number\n");
		return;
	}
	//actually execute the kill
	if(is_jid) {
		printf("kill by job %d\n", id);
	}
	else
		printf("kill by process %d\n", id);

	to_be_killed = (-1) * job->pgid;
	if(kill_flag)
		if(kill(to_be_killed,SIGKILL) == -1)
			perror("Kill failed\n");
	else
		if(kill(to_be_killed,SIGTERM) == -1)
			perror("Kill failed\n");

}


void put_job_in_foreground(Job* job) {
	pid_t pid;
	int status;
	//put the job into foreground;
	//shell_terminal is a global file descriptor represented as shell
	tcsetpgrp(myShTerminal, job->pgid);

	//stroe the current shell status to its termios
	//tcgetattr(myShTerminal, &myShTmodes);

	//restore the terminal attributes when the job stopped last time
	tcsetattr(myShTerminal, TCSADRAIN, &job->j_Tmodes);

	//we have already changed the status of process
	//wait foreground job to exit
	while(job->status != JOBCOMP && job->status != JOBSTOP) {
		//pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
		pid = waitpid(WAIT_ANY, &status, WUNTRACED);			// since it's in foreground, we shouldn't use WNOHANG
		printf("foreground job returned, pid is %d\n", pid);

		if(WIFSTOPPED(status))
			jobChangeStatus(job, JOBSTOP);
		else
			jobChangeStatus(job, JOBCOMP);

		printf("Job status is %d\n", job->status);
	}

	//printjob(job->id);
	//if the job complete, we exit the job
	if(job->status == JOBCOMP) {
		joblock(job); //need to implement, block all the possible access to job list
		jobRemoveJobId(job->id);
		jobunlock(job);
	}

	if(job->status == JOBSTOP) {
		//if job has been suspended, we store its termios
		tcgetattr(myShTerminal, &job->j_Tmodes);
	}

    /* Put the shell back in the foreground.  */
    tcsetpgrp (shell_terminal, shell_pgid);

    /* Restore the shell’s terminal modes.  */
    tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes); 

}


void bFg(char** args, int argn) {
	int jid; // get job id by args or use default setting
	Job* current_job = NULL;
	/*if(argn == 1)
		jid = num_of_job(); // implemented by jiaping, get the number of job in a job list
	else
		jid = atoi(args[1]); //convert the input job id to int

	current_job = getJobJobId(jid);
	if(current_job == NULL) {
		printf("job %d does not exist, we can find it\n", jid);
		return;
	}*/

	if (argn == 1) {
		current_job = getJLastBackgrounded();
	}
	else if (argn == 2){
		if (args[1][0] == '%' && atoi(args[1]+1) != 0) {
			current_job = getJobJobId(atoi(args[1]+1));
		}
		else {
			printf("Pleas type in %[number]\n", );
		}
	}
	else if(argn == 3) {
		if (args[1] == "%" && atoi(args[2]) != 0) {
			current_job = getJobJobId(atoi(args[2]));
		}
		else {
			printf("Pleas type in %[number]\n", );
		}
	}
	else {
		printf("Pleas type in %[number]\n", );	
	}

	//update job status
	job->field = JOBFORE;
	job->status = JOBRUN;
	pid_t current_pgid = -1 * job->pgid;
	if(kill(current_pgid, SIGCONT) < 0)
		perror("kill (SIGCONT)"); // send sigcont to all processes of that process group
	//we can directly send a signal to a process group
	/*
	while(current_job_process != NULL) {
		//need to implement kill()
		kill(current_job_process->pid, SIGCONT)； // send sigcont to each process in job
		current_job_process->status = PROCRUN; // set process to run status
		current_job_process = current_job_process->next;
	}
	*/


	put_job_in_foreground(current_job);

}

// could be "bg %        number" or "bg %number"
void bBg(char** args, int argn) {
	Job* current_job = NULL;
	/*if(argn == 1)
		jid = num_of_job(); // implemented by jiaping, get the number of job in a job list
	else
		jid = atoi(args[1]); //convert the input job id to int
	*/
	if (argn == 1) {
		current_job = getJLastSuspended();
	}
	else if (argn == 2){
		if (args[1][0] == '%' && atoi(args[1]+1) != 0) {
			current_job = getJobJobId(atoi(args[1]+1));
		}
		else {
			printf("Pleas type in %[number]\n", );
		}
	}
	else if(argn == 3) {
		if (args[1] == "%" && atoi(args[2]) != 0) {
			current_job = getJobJobId(atoi(args[2]));
		}
		else {
			printf("Pleas type in %[number]\n", );
		}
	}
	else {
		printf("Pleas type in %[number]\n", );	
	}

	//current_job = getJobJobId(jid);
	if(current_job == NULL) {
		printf("job %d does not exist, we can find it\n", jid);
		return;
	}

	//update job status
	job->field = JOBBACK;
	job->status = JOBRUN;
	pid_t current_pgid = -1 * job->pgid;
	if(kill(current_pgid, SIGCONT) < 0)
		perror("kill (SIGCONT)");


}
int check_built_in(Job* job) {
	char** args = job->processList->args;
	int argn = job->processList->argn;
	if(strcmp(args[0],"kill") == 0) {
		return TRUE;
	}
	else if (strcmp(args[0],"fg") == 0) {
		return TRUE;
	}
	else if (strcmp(args[0],"bg") == 0) {
		return TRUE;
	}
	else if (strcmp(args[0],"exit") == 0) {
		return TRUE;
	}
	else
		return FALSE;
}

int exeBuiltIn(char** args, int argn) {
	if(strcmp(args[0],"kill") == 0) {
		bKill(args, argn);
	}
	else if (strcmp(args[0],"fg") == 0) {
		bFg(args, argn)
	}
	else if (strcmp(args[0],"bg") == 0) {
		bBg(args, argn)
	}
	else if (strcmp(args[0],"exit") == 0) {
		bExit(); // todo
	}
	else
		perror("invalid input, check_built_in wrong probably\n");
}

// not supporting pipe for now.
// executing the command without pipe. Example: emacs shell.c; or emacs shell.c &
int executing_command_without_pipe(Job *job) {
	pid_t pid;
	int status;
	//Job *childjob;
	char** args = job->processList->args;
	int argn = job->processList->argn;
	// check if the this job is built-in command, foreground, or background
	// TODO: check_built_in(), exeBuiltIn(), if this job is built-in command
	if (check_built_in(job)) {
		exeBuiltIn(args, argn);
	}
	// If it is foreground job
	else if(job->field == JOBFORE) {
		pid = fork();
		if (pid == 0) {
			//child process
			// execute the command
			if((execvp(args[0], args)) == FAIL) {
				printf("Didn't execute the command: %s! Either don't know what it is, or it is unavailable. \n", command_line[0]);
      			exit(EXIT_FAILURE);
			}
		} else if (pid > 0) {
			// parent process
			waitpid(pid, &status, WUNTRACED);
			// if the signal is termination (WIFSIGNALED) or normal exit, remove the job and free the memory.
			if (WIFSIGNALED(status) || WIFEXITED(status)) {
				if(jobRemovePid(getpgid(pid)))
					printf("Job removal from the job list error.\n");
				freeJob(job);
			} else if (WIFSTOPPED(status)) {
			// else if the signal is stop, update the job's field, put it into background (save termios), 
				job->field = JOBBACK;
				tcgetattr(myShTerminal, &job->j_Tmodes);
			} else
				printf("Error in parent process");
		} else {
			perror("Forking error!");
			exit(EXIT_FAILURE);
		}
	} else if(job->field == JOBBACK) {
		// if this job is background job
		pid = fork();
		if (pid == 0) {
			//child process
			// execute the command
			if((execvp(args[0], args)) == FAIL) {
				printf("Didn't execute the command: %s! Either don't know what it is, or it is unavailable. \n", command_line[0]);
      			exit(EXIT_FAILURE);
			}
		} else if (pid > 0) {
			// parent process
			waitpid(pid, &status, WNOHANG);
			// if the signal is termination (WIFSIGNALED) or normal exit, remove the job and free the memory.
			if (WIFSIGNALED(status) || WIFEXITED(status)) {
				if(jobRemovePid(getpgid(pid)))
					printf("Job removal from the job list error.\n");
				freeJob(job);
			} else
				printf("Error in parent process");
		} else {
			perror("Forking error!");
			exit(EXIT_FAILURE);
		}
	}
}

void bExit() {
	freeJobList();
	exit(EXIT_SUCCESS);
}
