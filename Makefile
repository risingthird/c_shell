mysh: shell.c run_command.h util.h parser.h ll.h
	gcc -Wall -pedantic -g -o mysh shell.c run_command.c parser.c ll.c -lreadline
