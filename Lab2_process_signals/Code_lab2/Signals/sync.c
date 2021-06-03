#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

void siguser1_handler(int signo){
  
}

int main (){
  int i = 0, j = 0; 
  pid_t ret;
  int status;

  ret = fork();
  if(ret == 0){
    for (i = 0; i < 5000; i++){
      printf ("Child: %d\n", i);
      kill(getppid(), SIGUSR1);
      pause();
    }
    printf ("Child ends\n");
  }

  else{
    wait (&status);
    printf ("Parent resumes.\n");
    pause();
    for (j = 0; j < 5000; j++){
      printf ("Parent: %d\n", j);
      kill(ret, SIGUSR1);
      pause();
    }
  }

  return 0;
}
