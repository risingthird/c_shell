#include "util.h"

typedef struct Process_Node
{
    char **args;
    int argn;
    pid_t pid;
    int status;
    //struct Process_Node* prev;
    struct Process_Node *next;
} Process;

typedef struct bgpr
{
    char *line;
    int status;
    int jobId;
    pid_t pgid;
    Process *processList;
} Job;

typedef struct LinkedList
{
    Job *job;
    struct LinkedList *next;
    struct LinkedList *prev;
} Node;

Node *head;
Node *tail;
int size;
int id;

int jobRemovePid(pid_t pid);    // remove job by pid
int jobRemoveJobId(int job_id); // remove job by job id
int jobInsert(Job *toAdd);      // add the job to the job list
int jobInit();                  // initiate the job list.
Job *createJob(char *line, Process *process, int status);
Job *getJobJobId(int job_id); // get the current job by job id
Job *getJobPid(pid_t pid);    // get the current job by pid
void printList();             // print the list
//void printJob();
void freeArgs(char** args);
void freeProcess(Process *process);
void freeJob(Job *job);                     // free the job from memory
void freeJobList();                         // free the job list
void jobSetPGid(Job *job, pid_t pgid);      // set the process group id of the current job with a given pgid.
void jobChangeStatus(Job *job, int status); // change the status of the current job given a status code
