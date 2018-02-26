#include "run_command.h"

#define WAIT_ANY -1
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
	tcgetattr(myShTerminal, &myShTmodes);

	//restore the terminal attributes when the job stopped last time
	tcsetattr(myShTerminal, TCSADRAIN, &job->j_Tmodes);

	//we have already changed the status of process
	//wait foreground job to exit
	while(job->status != JOBCOMP && job->status != JOBSTOP) {
		pid = waitpid(WAIT_ANY, &statusm WUNTRACED | WNOHANG);
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
	if(argn == 1)
		jid = num_of_job(); // implemented by jiaping, get the number of job in a job list
	else
		jid = atoi(args[1]); //convert the input job id to int

	current_job = getJobJobId(jid);
	if(current_job == NULL) {
		printf("job %d does not exist, we can find it\n", jid);
		return;
	}

	//update job status
	job->status = JOBFORE;
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

void bBg(char** args, int argn) {
	Job* current_job = NULL;
	if(argn == 1)
		jid = num_of_job(); // implemented by jiaping, get the number of job in a job list
	else
		jid = atoi(args[1]); //convert the input job id to int

	current_job = getJobJobId(jid);
	if(current_job == NULL) {
		printf("job %d does not exist, we can find it\n", jid);
		return;
	}

	//update job status
	job->status = JOBFORE;
	pid_t current_pgid = -1 * job->pgid;
	if(kill(current_pgid, SIGCONT) < 0)
		perror("kill (SIGCONT)");


}