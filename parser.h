#include <stdlib.h>
#include <string.h>

char* readLine();
int parseCommands(char* line, char** command);
int parseSegments(char* command, char** segments);
int parseArguments(char* segments, char* arguments);