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
