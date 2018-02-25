#include "parser.h"


/*http://blog.csdn.net/lin535061851/article/details/48395607
void trimLeft(char *s)  
{  
    int i=0, j=0;  
    //传入空值则退出  
    if(!strlen(s)) return;  
    //找到首个不为空的字符  
    while( (s[i] == ' ' || s[i] == '\n' || s[i] == '\t') && s[i] != '\0') i++;  
    //从i位置的字符开始左移i个位置  
    while( s[i] != '\0') s[j++] = s[i++];  
    s[j] = '\0';  
}  

// trim all \t \n space to the right of the string
void trimRight(char *s)  
{  
    int pos;  
    pos = strlen(s) - 1;  
    //若尾部字符为空，则将其设置为末尾字符  
    while( s[pos] == ' ' || s[pos] == '\n' || s[pos] == '\t') {  
        s[pos--] = '\0';  
        if(pos<0) break;  
    }  
} 

void trimAll(char *s) {
	trimRight(s);
	trimLeft(s);
} */


int check_last_character_in_process(char* process_line) {
	int len = strlen(process_line);
	while(process_line[len] != '\0') {
		if(process_line[len] == ' ') {
			len--;
		}
		else if (process_line[len] == '&') {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	return FALSE;
}

// return -1 if it is not the last process in the line but has '&' sign as the last nonspace character
int parseCommands(char* line, char** command) {
	int count  = 0;
	int i = 0;
	int num_process = DEFAULT_NUM_PROCESS;
	char* split;

	if((split = strtok(line, ";")) == NULL) {
		count = 1;
		command[0] = line;
		return count;
	}

	while(split != NULL) {
		command[count] = split;
		count++;
		split = strtok(NULL, ";");
		if(count >= DEFAULT_NUM_PROCESS) {
				i++;
				num_process = DEFAULT_NUM_PROCESS << i;
				command = realloc(command, num_process * sizeof(char*));
		}
	}

	for (int j = 0; j < count - 1; ++j) {
		if (check_last_character_in_process(command[j])) {
			return -1;
		}
	}

	return count + 1;
}

int parseSegments(char* command, char** segments) {
	int count  = 0;
	int i = 0;
	int num_segments = DEFAULT_NUM_SEGMENTS;
	char* split;

	if((split = strtok(command, "|")) == NULL) {
		count = 1;
		segments[0] = command;
		return count;
	}

	while(split != NULL) {
		segments[count] = split;
		count++;
		split = strtok(NULL, ";");
		if(count >= num_segments) {
				i++;
				num_segments = DEFAULT_NUM_SEGMENTS << i;
				segments = realloc(segments, num_segments * sizeof(char*));
		}
	}

	return count + 1;
}

int parseArguments(char* segments, char** arguments) {
	int count  = 0;
	int i = 0;
	int num_arguments = DEFAULT_NUM_ARG;
	char* split;

	if((split = strtok(segments, "|")) == NULL) {
		count = 1;
		arguments[0] = segments;
		return count;
	}

	while(split != NULL) {
		segments[count] = split;
		count++;
		split = strtok(NULL, ";");
		if(count >= num_arguments) {
				i++;
				num_arguments = DEFAULT_NUM_ARG << i;
				segments = realloc(segments, num_arguments * sizeof(char*));
		}
	}

	return count + 1;
}





