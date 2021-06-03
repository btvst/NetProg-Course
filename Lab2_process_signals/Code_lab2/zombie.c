#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

// This command kills the parent process and all its zombies
// After the parent dies, the zombie will be inherited by pid 1, which will wait on it and clear its entry in the process table.

// kill -HUP $(ps -A -ostat,ppid | awk '{/[zZ]/{ print $2 }')

// This handeler is executed once only as pending signals of the same type are not executed,
// but are discarded.
void handle_child_exit(){ // 2 (3)
        wait(NULL);
        printf("Child exit handeler called\n");
}

void handle_interupt(){
        printf("Interupt handeler called\n");
}

int main(){
        signal(SIGCHLD, handle_child_exit); //2 (3)
        signal(SIGINT, handle_interupt);
        pid_t pid;

        pid=fork();

        if(pid<0)
                perror("fork:");
        else if(pid==0){
                printf("In Child: pid = %d, parent pid= %d\n", getpid(), getppid());
                printf("child finishing...\n");
                exit(0);
        }
        else if(pid>0){
                pid_t pid1 = fork();
                if(pid1 == 0){
                        printf("In Child: pid = %d, parent pid= %d", getpid(), getppid());
                        printf("child finishing...\n");
                        exit(0);
                }
                sleep(10);
                // while(1); Signals can be handeled by the handeler evn when the parent is in infinite loop
                // wait(NULL); // 2 (2)
        }

}
