#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "key.h"

struct msgbuff{
    long mtype;
    long dpid;
    long spid;
    char mtext[MAXSIZE];
};

int main(void){
    struct msgbuff buffer;
    int msqid;
    key_t key;
    
    if((key = ftok(MSGQ_PATH, 'C')) == -1){
        perror("ftok\n");
        exit(1);
    }

    if((msqid = msgget(key, 0644 | IPC_CREAT)) == -1){
        perror("msgget\n");
        exit(1);
    }

    fprintf(stdout, "My PID is: %d\n", getpid());
    fprintf(stdout, "Enter destination pid:\n");
    scanf("%ld", &buffer.dpid);
    getchar();

    fflush(stdout);
    fflush(stdin);

    buffer.mtype = 1;
    buffer.spid = getpid();
    initialize(buffer.mtext);
    printf("Ready to fork\n");

    pid_t pid = fork();
    if(pid == 0){
        fprintf(stdout, "Child process for %d\n", getppid());
        for(;;){
            if(msgrcv(msqid, &(buffer.mtype), sizeof(buffer), getppid(), 0) == -1){
                perror("msgrcv client\n");
            }

            fprintf(stdout, "client pid %d received from %ld ;; %s\n", getppid(), buffer.spid ,buffer.mtext);

            initialize(buffer.mtext);
        }
    }
    else if(pid > 0){
        sleep(5);
        fprintf(stdout, "Enter message:\n");
        while(gets(buffer.mtext), !feof(stdin)){
            if(msgsnd(msqid, &(buffer.mtype), sizeof(buffer), 0) == -1){
                perror("msgsnd client\n");
            }
            fprintf(stdout, "Enter message:\n");
            initialize(buffer.mtext);
        }
    }
    else{
        perror("fork client");
        exit(1);
    }
}