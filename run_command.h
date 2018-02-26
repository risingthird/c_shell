#include <signal.h>
#include <string.h>
#include <termios.h>
#include "ll.h"
#include "util.h"

void bKill(char** args, int argn);
void bFg(char** args, int argn);
void bBg(char** args, int argn);
int exeJ(Job* job);
int exeBuiltIn(char** args, int argn);
int changeDirectory(char** args);
