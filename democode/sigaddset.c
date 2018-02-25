/*
Kellie Dinh and Ju-Han Tarn
Sample program testing sigemptyset and sigaddset

This program registers a signal handler for the example signals SIGUSR1 and SIGUSR2
We add these signals to a sigset and then can call sigprocmask on the sigset

- Using SIG_UNBLOCK with the sigset will have the signal handler print out a message
- Using SIG_BLOCK with the sigset will block those signals so there won't be any messages printed
*/

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void printsth(int sig){
        if(sig == SIGUSR1){
                printf("%s\n", "SIGUSR1 from printsth" );
        }else if(sig == SIGUSR2){
                printf("%s\n", "SIGUSR2 from printsth");
        }else{
                printf("%s\n", "unknown signal...");
        }
}

int main(){
        // initialize sigaction struct and set handler as printsth()
        // call sigaction with SIGUSR1 signal and the struct we just created
        struct sigaction sct;
        sigemptyset(&sct.sa_mask);
        memset(&sct, 0, sizeof(sct));
        sct.sa_handler = &printsth;
        sigaction(SIGUSR1, &sct, NULL);

        struct sigaction sct2;
        sigemptyset(&sct2.sa_mask);
        //sct2.sa_flags = 0;
        memset(&sct2, 0, sizeof(sct2));
        sct2.sa_handler = &printsth;
        sigaction(SIGUSR2, &sct2, NULL);

        sigset_t sigset;

        // initializes empty signal set, required before modifying it in any way
        sigemptyset(&sigset);
        // adds SIGUSR1 and SIGUSR2 to sigset
        sigaddset(&sigset, SIGUSR1);
        sigaddset(&sigset, SIGUSR2);
        
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);

        printf("%s\n", "sigusr1 killed");
        kill(getpid(), SIGUSR1);
        printf("%s\n", "sigusr2 killed");
        kill(getpid(), SIGUSR2);

        sigprocmask(SIG_BLOCK, &sigset, NULL);

        printf("%s\n", "sigusr1 blocked");
        kill(getpid(), SIGUSR1);
        printf("%s\n", "sigusr2 blocked");
        kill(getpid(), SIGUSR2);
        return 0;
}
