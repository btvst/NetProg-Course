#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "key.h"

struct msgbuff{
    long mtype;
    long dpid;
    long spid;
    char mtext[MAXSIZE];
};

void copy(char *dest, char *src){
    int i=0;;
    while(i<MAXSIZE && src[i]!='\0'){
        dest[i] = src[i];
        ++i;
    }
}

int main(void){
    struct msgbuff buffer, sendbuffer;
    int msqid;
    key_t key;

    if((key = ftok(MSGQ_PATH, 'C')) == -1){
        perror("ftok");
        exit(1);
    }

    if((msqid = msgget(key, IPC_CREAT | 0644)) == -1){
        perror("msgget server");
        exit(1);
    }

    printf("server ready to receive messages\n");

    pid_t pid;
    for(;;){
        if(msgrcv(msqid, &(buffer.mtype), sizeof(buffer), 1, 0) == -1){
            perror("msgrcv server");
            exit(1);
        }

        sendbuffer.mtype = buffer.dpid;
        sendbuffer.dpid = buffer.dpid;
        sendbuffer.spid = buffer.spid;
        copy(sendbuffer.mtext, buffer.mtext);

        fprintf(stdout, "Server relayed message from %ld to %ld\n", buffer.spid, buffer.dpid);
        if(msgsnd(msqid, (void*)(&sendbuffer.mtype), sizeof(sendbuffer), 0) == -1){
            perror("msgsnd server");
            exit(1);
        }
        sendbuffer.mtext[0] = '\0';
    }
}