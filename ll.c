#include "ll.h"



void jobInit() {
	head = malloc(sizeof(Node));
	tail = malloc(sizeof(Node));
	head->prev = NULL;
	head->next = tail;
	tail->prev = head;
	tail->next = NULL;
	head->job = NULL;
	tail->job = NULL;
	id = 1;
}

Job* createJob(char* line, Process* process, int status, int field) {
	Job* toReturn = malloc(sizeof(Job));
	toReturn->status = status;
	toReturn->jobId = id;
	toReturn->field = field;
	id++;
	if(line != NULL) {
		toReturn->line  = malloc(sizeof(char) * strlen(line));
		strcpy(toReturn->line, line);
	}
	toReturn->processList = process;
	toReturn->pgid = 0;
	// I have no idea how to set pgid. Need to figure out later
	return toReturn;
}

Job* getJobJobId(int job_id) {
	Node* temp = head->next;
	while(temp != NULL && temp->job != NULL) {
		if(temp->job->jobId == job_id) {
			return temp->job;
		}
		else {
			temp = temp->next;
		}
	}
	return NULL;
}

Job* getJobPid(pid_t pid) {
	Node* temp = head->next;
	while(temp != NULL && temp->job != NULL) {
		if(temp->job->pgid == pid) {
			return temp->job;
		}
		else {
			temp = temp->next;
		}
	}
	return NULL;
}

Job* getJobCommandName(char* command) {
	Node* temp = head->next;
	int count = 0;
	Job* toReturn;
	while(temp != NULL && temp->job != NULL) {
		if(strstr(temp->job->line, command) != NULL) {
			toReturn = temp->job;
			count++;
		}
		else {
			temp = temp->next;
		}
	}
	return (count > 1 ? NULL : toReturn);
}

Job* getJLastSuspended() {
	return getJobJobId(last_suspended);
}

Job* getJLastBackgrounded() {
	return getJobJobId(last_backgrounded);
}

int jobInsert(Job* job) {
	Node* toAdd = malloc(sizeof(Node));
	if(!toAdd) {
		return FALSE;
	}
	toAdd->job = job;
	Node* temp = tail->prev;
	temp->next = toAdd;
	toAdd->prev = temp;
	toAdd->next = tail;
	tail->prev = toAdd;
	return TRUE;
}

int jobRemovePid(pid_t pid) {
	Node* temp = head->next;
	while(temp != NULL && temp->job != NULL) {
		if(temp->job->pgid == pid) {
			Node* temp2 = temp->prev;
			Node* temp3 = temp->next;
			temp2->next = temp3;
			temp3->prev = temp2;
			int i = temp->job->jobId;
			// update all job id after the deleted one
			while(temp2->job != NULL) {
				if (temp2->job->jobId > i) {
					temp2->job->jobId--;
				}
				temp2 = temp2->next;
			}
			id--;
			freeJob(temp->job);
			free(temp);
			return TRUE;
		}
		else {
			temp = temp->next;
		}
	}
	return FALSE;
}

int jobRemoveJobId(int job_id) {
	Node* temp = head->next;
	while(temp != NULL && temp->job != NULL) {
		if(temp->job->jobId == job_id) {
			Node* temp2 = temp->prev;
			Node* temp3 = temp->next;
			temp2->next = temp3;
			temp3->prev = temp2;
			int i = temp->job->jobId;
			// update all job id after the deleted one
			while(temp2->job != NULL) {
				if (temp2->job->jobId > i) {
					temp2->job->jobId--;
				}
				temp2 = temp2->next;
			}
			id--;
			freeJob(temp->job);
			free(temp);
			return TRUE;
		}
		else {
			temp = temp->next;
		}
	}
	return FALSE;
}

void jobSetPGid(Job* job, pid_t pgid) {
	job->pgid = pgid;
	return;
}

void jobChangeStatus(Job* job, int status) {
	job->status = status;
	return;
}

void printList() {
	Node* temp = head->next;
	while(temp != NULL && temp->job != NULL) {
		if (temp->job->status == JOBCOMP) {
			printf("[%d] Done                     %s\n", temp->job->jobId, temp->job->line);
		}
		else if(temp->job->status ==  JOBSTOP) {
			printf("[%d] Stopped                     %s\n", temp->job->jobId, temp->job->line);
		}
		else if(temp->job->status ==  JOBRUN) {
			printf("[%d] Run                     %s\n", temp->job->jobId,  temp->job->line);
		}
		else if (temp->job->status ==  JOBTERM) {
			printf("[%d] Forcefully Terminated                     %s\n", temp->job->jobId, temp->job->line);
		}
		else {
			perror("Print status error!\n");
			exit(EXIT_FAILURE);
		}
		temp = temp->next;
	}
}

void printJobStatus(Job* job) {
	if(job->status == JOBCOMP)
		printf("\n[%d] Done                     %s\n", job->jobId, job->line);
	else if(job->status == JOBSTOP)
		printf("\n[%d] Stopped                     %s\n", job->jobId, job->line);
	else if(job->status == JOBRUN)
		printf("\n[%d] Run                     %s\n", job->jobId, job->line);
	else if(job->status == JOBTERM)
		printf("\n[%d] Forcefully terminated                     %s\n", job->jobId, job->line);
	else {
		perror("Print status error!\n");
		exit(EXIT_FAILURE);
	}
}

void freeArgs(char** args) {
	int count = 0;
	while(args[count] != NULL) {
		free(args[count]);
		count++;
	}
	args = NULL;
	free(args);
}

void freeProcess(Process* process) {
	while(process != NULL) {
		Process* temp = process;
		process = process->next;
		freeArgs(temp->args);
		free(temp);
	}
}

void freeJob(Job* job) {
	freeProcess(job->processList);
	free(job->line);
	free(job);
}

void freeJobList() {
	while(head->next->job != NULL) {
		Node* temp = head->next;
		head->next = temp->next;
		freeJob(temp->job);
		free(temp);
	}
	free(head);
	free(tail);
}
