#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/* (b) Removing the sleep in Parent makes parent only write and consume all its own written messages */

/* (c) gives jumbled output */

/* (d) synchronization achieved by using 2 pipes */

#define MSGSIZE 16
int main(){
  int i;
  char *msg = "How are you?";
  char inbuff[MSGSIZE];
  
  int p_pc[2];
  int p_cp[2];

  pid_t ret;
  
  pipe(p_pc);
  pipe(p_cp);

  ret = fork();
  if(ret > 0){
    if(close(p_pc[0])==-1){
      printf("Parent read end close error\n");
      exit(0);
    }
    if(close(p_cp[1])==-1){
      printf("Parent write end close error\n");
      exit(0);
    }

    i = 0;
    while (i < 10){
      printf("------------------------------------\n");
      printf("Parent writing message %d th time\n", i);
      write (p_pc[1], msg, MSGSIZE);
      printf("------------------------------------\n");
      sleep (2);
      read (p_cp[0], inbuff, MSGSIZE);
      printf ("ith time: %d Parent: %s\n",i, inbuff);
      i++;
    }
    exit(1);
  }
  else{
    if(close(p_pc[1])==-1){
      printf("Child write end close error\n");
      exit(0);
    }
    if(close(p_cp[0])==-1){
      printf("Child read end close error\n");
      exit(0);
    }

    i = 0;
    while(i < 10){
      sleep (1);
      read(p_pc[0], inbuff, MSGSIZE);
      printf ("ith time: %d Child: %s\n",i, inbuff);
      printf("#####################################\n");
      printf("Child writing message %d th time\n", i);
      write (p_cp[1], "i am fine", strlen("i am fine"));
      printf("#####################################\n");
      i++;
    }
  }
  exit (0);
}