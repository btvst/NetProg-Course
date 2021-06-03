#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

// Signals can be received by a process even when it is in an infinite

void handler(int signo){
    printf("SIGUSR1 signal received\n");
    exit(1);
}

int main(void){
    signal(SIGUSR1, handler);
    printf("PID: %d\n", getpid());
    for(;;){

    }
}