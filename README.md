Name: Jiaping Wang, Zhanpeng Wang, Tianming (Mark) Xu homework 3

Basic structure of our shell:
	In our shell, we support most of commands in a normal shell, but we don't support redirection of file and pipe. We also support several built in functions including "fg", "bg", "kill flag job number" and "exit". Some limitations will be stated below. We divided the project into four parts, joblist, parser, built-in commands and main shell loop. We use individual file to contain each part. Basically, user can type command into our shell, if it is a built-in command, we will identify that and run it in parent process; if it is not a built-in command, we will fork a new child to execute that command. The shell also supports foreground and background jobs. In the main shell, we have a signal handler which helps us deal with the SIGCHLD signal sent by terminated or suspended child process. 

How to compile: 
	I have written a Makefile, the default "make" command will let you compile the mysh program. I also have a "make clean" command to delete all files except the necessary .h .c files.

How to run it:
	 For running mysh program, only one default option: "./mysh", and if you see the prompt: "[用心写bug，用脚写shell, (ヘ･_･)ヘ┳━┳ ┫ ┻━┻ ︵ヽ(`Д´)ﾉ︵ ┻━┻ ┣ﾍ(^▽^ﾍ)Ξ(ﾟ▽ﾟ*)ﾉ┳━┳, ¯\\_(ツ)_/¯ TZJ Shell]" you are in my shell now.

Feature list with known bugs and limitations:
	1. The limitation of fg is that user can only input "fg" with less than or equal to one '%'. For exmaple, "fg %1 %2" is not a valid command and we will print an error message. However, we support white space between '%' and the job number. If there is no '%', we will first assume it is also a job and search. Also, we don't support bring more than one job back to foreground. That is to say "fg 1 2 3" is invalid. We will also report this error to user.
	2. The limitation of bg is similar to fg, we don't support more than one '%' and multiple job ids.
	3. kill has more features. In kill, you can send signals to multiple jobs or processes. But our kill only has two options, "-9" flag and no flag. If user choose to put a "-9" after kill, we will send the SIGKILL to the process or job she provides. If no flag is given, we will send SIGTERM to jobs. Be noticed that the flag can only provide right after the kill command("kill -9 %1"). Otherwise, you will get a invalid use warning. We also support the white space between '%' and job number.
	4. User can type exit to exit our shell.
	5. Our shell supports tab completion
	6. Our shell supports the feature of changing directory.
	7. Our shell doesn't support redirect files, pipe.
	

file directory:
	It is a complicated project, so we have several extra files than our main program, shell.c
	1. ll.h and ll.c . These two files are where we define and implement our linked list, job struct and all the linked list and free function we used in our main program. 
	2. parser.c and parser.h . These two files are where our parser defined and implemented. 
	3. run_command.h and run_command.c . These two files are where we define and implement all the built in functions and execution of commands.
	4. util.h . In this header file, we define several important global variables and include most of the necessary libraries we need in other files.
	While we were doing this project, we got help from a lot of people and webpages, tutorials, such as: 
				https://github.com/jmreyes/simple-c-shell/blob/master/simple-c-shell.c 
				https://github.com/stpddream/OSHomework/tree/master/hw3
				http://blog.csdn.net/lin535061851/article/details/48395607
				Ziting Shen, Linyi Chen, Xuan Huang and Elieen Feng.

Testing:
	We used several combinations of commands to make sure our shell is robust enough to handle any reasonable requests from user. ( 'A -> B' means input A , press enter, then input B )
	1. sleep 10 & -> fg -> ctrl-c -> exit
	This combination checks whether our fg works properly and whether child can get sigINT normally
	2. emacs ll.h & -> jobs -> fg -> ctrl-z -> jobs -> bg -> jobs -> fg -> ctrl-c -> exit
	This combination checks whether jobs, fg and bg work properly, whether control-z can suspend job. Jobs also helps us see wether the change of status of job is what we want.
	3. sleep 100 & *5 -> jobs -> kill -9 1 2 3 4 5 -> jobs -> exit
	This combination checks whether our kill works properly and able to send signal to several process(jobs).
	4. cat ll.h ; ls ; cat ll.h ; ls ; ... cat ll.h
	This combination checks whether semicolon can seperate commands.
	5. other kinds of test testing whether fg, bg and kill support white space between '%' and number.
Since our shell gave us expected result of all these tests without any memory leak and memory error, we believe our shell is robust enought to handle any reasonable request from user.
