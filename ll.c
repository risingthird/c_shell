#include "ll.h"



int jobInit() {
	head = malloc(sizeof(LinkedList));
	tail = malloc(sizeof(LinkedList));
	head->prev = NULL;
	head->next = tail;
	tail->prev = head;
	tail->next = NULL;
	head->job = NULL;
	tail->job = NULL;
	int id = 0;
}

Job* createJob(char* line, Process* process, int status) {
	Job* toReturn = malloc(sizeof(Job));
	toReturn->status = status;
	toReturn->jobId = id;
	id++;
	if(line != NULL) {
		toReturn->line  = malloc(sizeof(char) * strlen(line));
		strcpy(toReturn->line, line);
	}
	toReturn->processList = process;
	toReturn->pgid = -1;
	// I have no idea how to set pgid. Need to figure out later
	return toReturn;
}

Job* getJobJobId(int job_id) {
	Node* temp = head;
	while(temp->next->job != NULL) {
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
	Node* temp = head;
	while(temp->next->job != NULL) {
		if(temp->job->pgid == pid) {
			return temp->job;
		}
		else {
			temp = temp->next;
		}
	}
	return NULL;
}

int jobInsert(Job* job) {
	Node* toAdd = malloc(sizeof(Node));
	if(!toAdd) {
		return -1;
	}
	toAdd->job = job;
	Node* temp = tail->prev;
	temp->next = toAdd;
	toAdd->prev = temp;
	toAdd->next = tail;
	tail->prev = toAdd;
	return 1;
}

int jobRemovePid(pid_t pid) {
	Node* temp = head;
	while(temp->next->job != NULL) {
		if(temp->job->pgid == pid) {
			Node* temp2 = temp->prev;
			Node* temp3 = temp->next;
			temp2->next = temp3;
			temp3->prev = temp2;
			freeJob(temp->job);
			free(temp);
			return 1;
		}
		else {
			temp = temp->next;
		}
	}
	return -1;
}

int jobRemoveJobId(int job_id) {
	Node* temp = head;
	while(temp->next->job != NULL) {
		if(temp->job->jobId == job_id) {
			Node* temp2 = temp->prev;
			Node* temp3 = temp->next;
			temp2->next = temp3;
			temp3->prev = temp2;
			freeJob(temp->job);
			free(temp);
			return 1;
		}
		else {
			temp = temp->next;
		}
	}
	return -1;
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
	Node* temp = head;
	while(temp->next->job != NULL) {
		if (temp->next->job->status == 0) {
			printf("[%d] Done                     %s", temp->next->job->jobId, temp->next->job->line);
		}
		else if(temp->next->job->status ==  1) {
			printf("[%d] Stopped                     %s", temp->next->job->jobId, temp->next->job->line);
		}
		else if(temp->next->job->status ==  2) {
			printf("[%d] Run                     %s", temp->next->job->jobId, temp->next->job->line);
		}
		else {
			perror("Print status error!\n");
			exit(1);
		}
		temp = temp->next;
	}
}

void freeProcess(Process* process) {
	while(process->next != NULL) {
		Process* temp = process;
		process = process->next;
		free(temp->args);
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
		LinkedList* temp = head->next;
		head->next = temp->next;
		freeJob(temp->job);
		free(temp);
	}
	free(head);
	free(tail);
}



























