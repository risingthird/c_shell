#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>


#define DEFAULT_NUM_PROCESS 2
#define DEFAULT_NUM_SEGMENTS 2
#define DEFAULT_NUM_ARG 2
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