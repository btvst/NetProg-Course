#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define key 20170854

int main(){
  int id;
  id = semget(key, 1, IPC_CREAT | 0666);
  if(id < 0){
    printf ("Semaphore is not created\n");
  }
  
  printf("Semaphore successfully created with id %d\n", id);

  int ret = semctl(id, 0, SETVAL,32768);
  if(ret != -1){
    printf("Semaphore set value: %d\n", ret);
  }
  else{
    perror("Value not allowed\n");
  }
  
  return 0;
}
