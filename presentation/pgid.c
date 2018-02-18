/* Modify the codes from IBM knowledge center: 
https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rtsetp.htm
https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rtgtpg.htm
 */
#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

main() {
  int status;
  pid_t pid;
  if ((pid=fork()) == 0) {
    if (fork() == 0) {
      printf("grandchild's pid is %d, process group id is %d\n",
             (int) getpid(), (int) getpgrp());
      exit(0);
    }
    printf("child's pid is %d, process group id is %d\n",
           (int) getpid(), (int) getpgrp());
    wait(&status); //wait grand child to exit
    exit(0);
  }
  printf("parent's pid is %d, process group id is %d\n",
         (int) getpid(), (int) getpgrp());
  printf("parent is performing setpgid() on pid %d\n", (int) pid);
  if(setpgid(pid,0) != 0)
    perror("setpgid() error");
  printf("parent's process group id is now %d\n", (int) getpgrp());
  printf("the parent's parent's pid is %d, process group id is %d\n", (int) getppid(), (int) getpgid(getppid())); //using pid to get process group id
  wait(&status);
}
