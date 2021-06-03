#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/* (a) wait is required, otherwise the ec command will execute first */
 
/* (b) If we do not close(p[1]) in parent, then the write end will remain open and read will be blocked forever */

/* ls has its stdout connected to fd 1 in the process table, whereas wc has its stdin connected to fd 0 in the process table */

/* Child process executes first */

int main(){
  int i;
  int p1[2];

  pid_t ret;
  pid_t ret1;

  pipe(p1);
  ret = fork();

  if(ret == 0){
    close(1);
    dup(p1[1]);
    close(p1[0]);
    execlp("ls", "ls", "-l", (char *) 0);
  }

  if(ret > 0){
    int p2[2];
    pipe(p2);
    int ret1 = fork();
    if(ret1==0){
      close(0);
      dup(p1[0]);
      
      close(p1[1]);
      close(p2[0]);

      close(1);
      dup(p2[1]);

      wait(NULL);
      execlp("grep", "grep", "^d", (char*) 0);
    }
    else{
      close(p1[0]);
      close(p1[1]);

      close(p2[1]);
      close(0);
      dup(p2[0]);

      wait(NULL);
      execlp("wc", "wc", "-l", (char*) 0);
    }
    // close(0);
    // dup(p[0]);
    // close(p[1]);
    // wait(NULL);
    // execlp("wc", "wc", "-l", (char *) 0);
  }
}