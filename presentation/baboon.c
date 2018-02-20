/* inspired by this tutorial: http://blog.superpat.com/2010/07/14/semaphores-on-linux-sem_init-vs-sem_open/
   and this github page: https://github.com/stpddream/OSHomework/blob/master/hw3/baboon_process_31/main.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

#define BABOON_NUM 10

sem_t* rope; //controls critical region rope
sem_t* eb; //controls eastward baboon counter
sem_t* wb; //controls westward baboons
int* num_e; //if we use this, all processes will have the same result
int* num_w;
//int* num_on_rope;
enum direction{west,east};
enum status{arrives, on_rope, crossed};
typedef struct Baboon{
  int id;
  int direction;
  int status;
}baboon;

void print_info(baboon* b) {
  if(b->direction){
    switch(b->status) {
    case arrives:
      printf("baboon %d goes eastward arrives\n",b->id);
      break;
    case on_rope:
      printf("baboon %d goes eastward is on the rope\n",b->id);
      break;
    case crossed:
      printf("baboon %d goes eastward has crossed the rope\n",b->id);
      break;
    default:
      printf("invalid status\n");
    }
  }
  else {
    switch(b->status) {
    case arrives:
      printf("baboon %d goes westward arrives\n",b->id);
      break;
    case on_rope:
      printf("baboon %d goes westward is on the rope\n",b->id);
      break;
    case crossed:
      printf("baboon %d goes westward has crossed the rope\n",b->id);
      break;
    default:
      printf("invalid status\n");
    }
    
  }
}

void toward_east(baboon* b) {
  sem_wait(eb);
  *num_e = *num_e + 1;
  b->status = arrives;
  print_info(b);
  printf("num of eastward baboon after baboon %d arrvies is now %d\n",b->id, *num_e);
  //b->status = arrives;
  //print_info(b);
  if(*num_e == 1)
    sem_wait(rope);
  sem_post(eb);
  sleep(1); //get on the rop   
  b->status = on_rope;
  print_info(b);
  sleep(4); //traverse the rope
  b->status = crossed;
  print_info(b);
  sem_wait(eb);
  *num_e = *num_e - 1;
  printf("eastward baboon number after baboon %d crossed is now %d\n",b->id, *num_e);
  if(*num_e == 0)
    sem_post(rope);
  sem_post(eb);
}

void toward_west(baboon* b) {
  sem_wait(wb);
  *num_w = *num_w + 1;
  b->status = arrives;
  print_info(b);
  printf("num of westward baboon after baboon %d arrvies is now %d\n", b->id, *num_w);
  //b->status = arrives;
  //print_info(b);
  if(*num_w == 1)
    sem_wait(rope);
  sem_post(wb);
  sleep(1); //get on the rop   
  b->status = on_rope;
  print_info(b);
  sleep(4); //traverse the rope
  b->status = crossed;
  print_info(b);
  sem_wait(wb);
  *num_w = *num_w - 1;
  printf("westward baboon after baboon %d crossed is now %d \n",b->id, *num_w);
  if(*num_w == 0)
    sem_post(rope);
  sem_post(wb);
  
}

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_ANONYMOUS | MAP_SHARED;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, 0, 0);
}


int main() {
  /*
  if(sem_init(&rope,1,1)){
    perror("sem rope init fails\n");
    exit(1);
  }
  if(sem_init(&eb,1,1)) {
    perror("sem eb init fails\n");
    exit(1);
  }
  if(sem_init(&wb,1,1)) {
    perror("sem wb init fails\n");
    exit(1);
  }
  */
  //in case previous semaphore didn't unlink
  sem_unlink("rope semaphore");
  sem_unlink("west_ward baboon semaphore");
  sem_unlink("east_ward baboon semaphore");
  
  //should use sem_open instead
  rope = sem_open("rope semaphore", O_CREAT,S_IRUSR | S_IWUSR, 1);
  wb = sem_open("west_ward baboon semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1);
  eb = sem_open("east_ward baboon semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1);
  //char parent_message[] = "hello";  // parent process will write this message
  //char child_message[] = "goodbye"; // child process will then write this one
  //initialize the rand generator
  time_t t = time(NULL);
  pid_t pid;
  int i; // id of baboon
  num_e = (int*)create_shared_memory(4);
  num_w = (int*)create_shared_memory(4);
  *num_e = 0;
  *num_w = 0;
  //num_on_rope = (int*)create_shared_memory(4);
  //*num_on_rope = 0;
  //printf("num_e now is %d\n",*num_e);
  //printf("num_w now is %d\n",*num_w);
  //memcpy(shmem, parent_message, sizeof(parent_message));
  for(i = 0; i < BABOON_NUM; i++) {
    srand((unsigned) time(NULL) + i);
    int sleep_time = rand()%6 + 1;
    pid = fork();
    if (pid <0) {
      perror("fail to create new process\n");
      exit(1);
    }
    else if (pid == 0){
      //printf("baboon %d has been created\n",i);
      break;
    }
    //printf("we will sleep %d seconds\n", sleep_time);
    sleep(sleep_time);
  }

  if (pid != 0) {
    //wait for all baboon to go across the rope
    while(pid = waitpid(-1, NULL,0)) {
      if(errno == ECHILD)
	break;
    }
    printf("\nAll baboons went across the rope\n");
    sem_unlink("rope semaphore");
    sem_unlink("west_ward baboon semaphore");
    sem_unlink("east_ward baboon semaphore");
  }
  else {
    int rand_result = rand();
    //printf("randresult is %d\n",rand_result);
    int direction = rand_result%2;
    baboon* b = malloc(sizeof(baboon));
    b->id = i;
    b->direction = direction;
    if(direction) {
      //printf("this baboon goes east\n");
      toward_east(b);
    }
    else {
      //printf("this baboon goes west\n");
      toward_west(b);
    }
    free(b);
    exit(0);
  }
