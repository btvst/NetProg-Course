#include <stdio.h>     
#include <sys/types.h> 
#include <sys/ipc.h>   
#include <sys/msg.h>
#include <stdlib.h>

int main(){
    int queue_id = msgget(IPC_PRIVATE, 0600); 
    if (queue_id == -1){
        perror("msgget");
        exit(1);
    }

    return 0;
}
