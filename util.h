#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>


#define DEFAULT_NUM_PROCESS 1024
#define DEFAULT_NUM_SEGMENTS 1024
#define DEFAULT_NUM_ARG 1024

enum {FALSE, TRUE}; // FALSE 0, TRUE 1;
enum {JOBCOMP, JOBSTOP, JOBRUN, JOBBACK, JOBFORE, JOBTERM}; // JOBCOMP 0, JOBSTOP 1, JOBRUN 2, JOBBACK 3, JOBFORE 4, JOBTERM 5
enum {PROCCOMP, PROCSTOP, PROCRUN}; // PROCCOMP 0, PROCSTOP 1, PROCRUN 2
/*
#define TRUE 1
#define FALSE 0
#define JOBCOMP 0
#define JOBSTOP 1 
#define JOBRUN 2
#define JOBBACK 3
#define JOBFORE 4
#define PROCCOMP 0
#define PROCSTOP 1
#define PROCRUN 2
*/
