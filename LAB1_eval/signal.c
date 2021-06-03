#include "signal_lab1.h"

int sigusr1 = 0;

void receiveTimerSignal(){
    int i=0;
    while(i<10){ // Each of the process, continously and for infinite time, get an alarm signal every 2 seconds.
        printf("[process] %d, i: %d\n", getpid(), i);
        alarm(2);
        sleep(2);
        ++i;
    }
    if(sigusr1==0){
        printf("For [process] id %d, SIGUSR1 count=0\nExiting now\n", getpid());
        printf("\n");
        exit(0);
    }
    else{
        printf("For [process] id %d, SIGUSR1 count!=0\nExit by normal flow after 5 seconds\n", getpid());
        printf("\n");
        sleep(5);
    }
}

void handle_alarm(){
    printf("SIGALRM received by process %d\n", getpid());
    for(int i=1; i<=12; ++i){
        int does_exist = kill(getpid()+i, 0); //return 1 if process with pid+i exist in the OS queue.
        if(does_exist == 0){
            printf("AAAA: For [process]: %d, [process] with pid+ %d: %d exists\n", getpid(), i, getpid()+i);
            kill(getpid()+i, SIGUSR1); // Send SIGUSR1 signal to existing  processes
        }
        else{
            // printf("For [process]: %d, [process] with pid+ %d: %d does not exists\n", getpid(), i, getpid()+i);
        }
    }
}

void counter(int signo){
    if(signo==SIGUSR1){
        ++sigusr1;
        printf("[process] %d, SIGUSR1 count: %d\n", getpid(), sigusr1);
    }
}

void checkNext12(int pid){
    for(int i=1; i<=12; ++i){
        int does_exist = kill(getpid()+i, 0); //return 1 if process with pid+i exist in the OS queue.
        if(does_exist == 0){
            printf("BBBB: For [process]: %d, [process] with pid+ %d: %d exists\n", pid, i, pid+i);
        }
    }
}

void createZProcesses(int z){
    for(int i=0; i<z; ++i){
        int child_pid = fork();
        if(child_pid==0){ //Both process and its child start execution from here after forking
            printf("CCCC: [grandson]: %d -- [parent]: %d\n", getpid(), getppid());
            checkNext12(getpid());
            receiveTimerSignal();
            exit(0); //To exit by normal flow otherwise, these processes will produce their child, not desirable.
        }
    }
    for(int i=0; i<z; ++i){ //wait for z child processes to terminate
        wait(NULL);
    }
}

void createProcesses(int n){
    // After the loop runs, n times, n child processes from the same parent will be created.
    for(int i=0; i<n; ++i){
        if(fork()==0){ //Both parent and its n children start execution from here after forking
            printf("[son] id: %d -- [parent] id: %d\n",getpid(), getppid());
            createZProcesses(getpid()%2);
            checkNext12(getpid());
            receiveTimerSignal();
            exit(0);  //To exit by normal flow otherwise, these processes will produce their child, not desirable.
        }
    }
    int status;
    for(int i=0; i<n; ++i){ //wait for n child process to terminate
        wait(&status);
    }
}


int main(int argc, char *argv[]){
    signal(SIGALRM, handle_alarm);
    signal(SIGUSR1, counter);

    int n;
    printf("Enter n:\n");
    scanf("%d", &n);
    createProcesses(n);
}
