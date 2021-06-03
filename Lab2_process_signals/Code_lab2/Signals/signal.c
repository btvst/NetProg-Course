#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

void int_handler(int signo);

void stop_handler(int signo){
  printf("Running stop_handler\n");
}

void quit_handler(int signo){
  printf("Running quit_handler\n");
}

int main (){
  sigset_t newMask, oldMask;
  sigemptyset(&newMask);
  sigemptyset(&oldMask);
  sigaddset(&newMask, SIGINT);
  sigaddset(&newMask, SIGQUIT);

  sigprocmask(SIG_BLOCK, &newMask, &oldMask);

  // signal(SIGINT, int_handler);
  // signal(SIGSTOP, stop_handler);  // The signals SIGSTOP and SIGKILL can't be caught, blocked or ignored.

  // signal(SIGQUIT, quit_handler);
  // signal(SIGINT, SIG_DFL); // Here, Ctrl+C exits the process

  printf ("Entering infinite loop\n");
  
  sleep (10);
  
  printf (" This is unreachable\n");
}

/* will be called asynchronously, even during a sleep */
void int_handler(int signo){
  printf ("Running int_handler\n");
}