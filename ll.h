#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>


#define JOBSUSP 0
#define JOBSTOP 1 
#define JOBRUN 2
#define JOBBACK 3
#define JOBFORE 4
#define PROCSUSP 0
#define PROCSTOP 1
#define PROCRUN 2

typedef struct Process_Node {
    char** args;
    int argn;
    pid_t pid;
    int status;
    //struct Process_Node* prev;
    struct Process_Node* next;
} Process;

typedef struct bgpr
{
	char* line;
	int status;
	int jobId;
	pid_t pgid;	
	Process* processList;
} Job;

typedef struct LinkedList {
    Job* job;
    struct LinkedList* next;
    struct LinkedList* prev;
} Node;

Node* head;
Node* tail;
int size;
int id;

int jobRemovePid(pid_t pid);
int jobRemoveJobId(int job_id);
int jobInsert(Job* job);
int jobInit();
Job* createJob(char* line, Process* process, int status);
Job* getJobJobId(int job_id);
Job* getJobPid(pid_t pid);
void printList();
void printJob();
void freeProcess(Process* process);
void freeJob(Job* job);
void freeJobList();
void jobSetPGid(Job* job, pid_t pgid);
void jobChangeStatus(pid_t pgid, int status); //used in signal handler and after we get the SIGCHILD we will let it sleep 0.01s to let shell create new process first.

