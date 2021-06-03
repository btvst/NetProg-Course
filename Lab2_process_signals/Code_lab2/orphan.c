#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

/* Orphan process is not shown in the process table ps -al */

/* Orphan process has parent id 1630 i.e. not 1 */

int main(){  
    int pid;
    pid=getpid();

    printf("Current Process ID is : %d\n",pid);

    printf("[ Forking Child Process ... ] \n");    
    pid=fork(); 
    if(pid < 0){
        /* Process Creation Failed ... */
        exit(-1);
    }
    else if(pid==0){
        /* Child Process */
        printf("Child Process is Sleeping ...");
        sleep(5);
        /* 
         * Orphan Child's Parent ID is 1                            
         */
        printf("\nOrphan Child's Parent ID : %d",getppid());
    }
    else{
        /* Parent Process */
        printf("Parent Process Completed ...");
    }

    return 0;
}