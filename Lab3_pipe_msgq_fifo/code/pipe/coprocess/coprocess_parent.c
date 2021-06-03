#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define MAXSIZE 100

int main(){

  int p1[2], p2[2];
  int ret;
  FILE *fpi, *fpo;
  char line[MAXSIZE], result[MAXSIZE];

  pipe(p1);
  pipe(p2);

  ret = fork ();
  if(ret == 0){
    printf("Child pid: %d\n", getpid());
    close(p1[1]);
    close(p2[0]);

    dup2(p1[0], 0);
    dup2(p2[1], 1);

    execl("./validateEmail", "validateEmail", (char *)0);
  }
  else{
    printf("Parent pid: %d\n", getpid());
    // int status = 0;
    // pid_t child_pid = wait(&status);

    // if(WIFEXITED(status) == SIGKILL){
    //   printf("exited with %d\n", WEXITSTATUS(status));
    // }
    // else if(WIFSIGNALED(status) == SIGQUIT){
    //   printf("signalled with %d\n", WTERMSIG(status));
    // }

    close(p1[0]);
    close(p2[1]);

    fpi = fopen("emails.txt", "r");
    fpo = fopen("emails_validation.txt", "w");

    while (fgets(line, MAXSIZE, fpi) != NULL){
      write(p1[1], line, strlen (line));
      read(p2[0], result, MAXSIZE);
      fprintf(fpo, "%s,%d\n", line, atoi (result));
    }
  }

  return 0;
}