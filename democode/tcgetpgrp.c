//Name: Jiayi(Rose) Lin and Eileen Feng
//This design of code aims at showing how tcgetpgrp() and tcsetpgrp() system call watk.
// Design:
// - fork() a child process that runs in the background.
// - show both child process pid and process group id.
// - show both parent proces pid and process group id.
// - use tcgetpgrp(STDOUT_FILENO) to get the process group id of the foreground process
// - use tcsetpgrp(STDOUT_FILENO, ) to set the background process group id to foreground process
//   group id
// ------------------------INSIGHT 1------------------------------
// - If the process group id of the background process(child process) has changed, cannot
//   simply use tcsetpgrp() to change the group id back to the group id of the foreground
//   process. Because recovery of the terminal attibute should come in.


# include <sys/types.h>
# include <unistd.h>
# include <stdio.h>
# include <wait.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>

int main(int argc, char** argv){
  pid_t pid;
  pid_t pgid;
  pid = fork();
  if(pid < 0) {
    printf("fork failed\n");
  } else if(pid == 0) {
    pid_t childpid =  getpid();
    pid_t childpgid = getpgid(getpid());
    printf("Child group id is: %d\n", childpgid);
    printf("Child pid is: %d\n", childpid);
    //-------------------------INSIGHT 1--------------------------------
    // if (setpgid(childpid, 0) != 0){
    //   perror("setpgid() failed\n");
    // } else {
    //   printf("child group id reset to: %d\n",(int) getpgid(getpid()));
    // }
    //-------------------------END OF INSIGHT 1------------------------
    pid_t foregroundpgid;
    if ((foregroundpgid = tcgetpgrp(STDOUT_FILENO)) < 0){
      perror("tcgetpgrp() failed");
    }else{
      printf("The current foreground process group id of STDOUT: %d\n", (int)foregroundpgid);
    }

    printf("Now try to set child process group to foreground\n");
    printf("Child group id is: %d\n", getpgid(getpid()));
    if ( tcsetpgrp(STDOUT_FILENO, getpgid(getpid())) == -1){
      printf("%s\n", strerror(errno));
      if(errno == EBADF) {
        printf("fd not valid\n");
      } else if (errno == EINVAL) {
        printf("pgrp unsupported value");
      } else if (errno == ENOTTY) {
        printf("calling process does not have a controlling terminal\n");
      } else if (errno == EPERM) {
        printf(" pgrp not matched with calling process \n");
      }
    }else {
        pid_t nowfg = tcgetpgrp(STDOUT_FILENO);
        printf("Now the child group %d is set as foreground process\n", nowfg);
    }
    exit(0);
  } else if (pid > 0) {
    int stat;
    pid_t old = getppid();
    printf("Parent pid is: %d\n", old);
    pid_t old_group = getpgid(0);
    printf("Parent group id: %d\n", old_group);
    int re;
    if((re = waitpid(-1, &stat, WNOHANG)) == 0) {
      printf("All children returned\n");
    }
  }
}
