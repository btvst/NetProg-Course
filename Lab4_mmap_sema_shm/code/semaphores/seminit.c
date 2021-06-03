#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "semun.h"
#include <sys/errno.h>

#define key (2000)

/* Maximum value to which a semaphore can be set is 32767 */

int sema(int _value){
  int semid, cnt, ret;
  semid = semget(key, 1, IPC_CREAT | 0666);

  if(semid >= 0){
    union semun arg;
    struct sembuf sop;

    arg.val = 0;

    /* Read the current semaphore count */
    cnt = semctl(semid, 0, GETVAL);
    if(cnt != -1){
      printf ("sem count read: current semaphore count %d.\n", cnt);
    }
    else
      perror("semctl getval:");
    
    /*set sem value*/
    if(semctl(semid, 0, SETVAL, _value) == -1){
      perror("semctl");
    }

    sop.sem_num = 0;
    sop.sem_op = 0;
    sop.sem_flg = 0;
    if(semop(semid, &sop, 1) == -1){
      perror("semop");
    }
  }
  else{
    const int MAX_TRIES = 10;
    int j;
    union semun arg1;
    struct semid_ds ds;

    if(errno != EEXIST){
      perror("semget");
    }

    semid = semget(key, 1, IPC_CREAT|IPC_EXCL);
    if(semid == -1){
      perror("semget");
    }

    arg1.__buf = &ds;
    for(j=0; j<MAX_TRIES; ++j){
      if(semctl(semid, 0, IPC_STAT, arg1)==-1){ //IPC_STAT stores the semid_ds for semid into (arg1.__buf == ds)
        perror("semctl");
      }
      if(ds.sem_otime!=0){
        break;
      }
      sleep(1);
    }   
  }
  return 0;
}

int main(){
  // int low = 0;
  // int high = __INT_MAX__;

  // int mid;
  // while(low <= high){
  //   int mid = (low+high)/2;
  //   printf("mid: %d\n", mid);
  //   printf("high: %d\n", high);
  //   printf("low: %d\n", low);
  //   int val = sema(mid);

  //   if(val == -1){
  //     high = mid-1;
  //   }
  //   else{
  //     low = mid+1;
  //   }
  // }
  
  // printf("Max sem val: %d\n", mid);

  sema(1);
}
