#include <signal.h>
#include <string.h>
#include <termios.h>
#include "ll.h"
#include "util.h"

void bKill(char** args, int argn);
void bFg(char** args, int argn);
void bBg(char** args, int argn);
void bExit();
void bJobs();
int executing_command_without_pipe(Job* job);
int exeBuiltIn(char** args, int argn);
int changeDirectory(char** args);
int check_built_in(Job* job);
