/*webserve.c*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

int childPIDs[100]; //Maximum 100 childs
int ind = 0;

void handler(int signo){
  int status=-1; //status changes to 0 upon child termination
  // printf("####################################################################\n");
  // printf("Status value: %d\n", status);
  // printf("####################################################################\n");
  // printf("Child exited\n");
  // printf("------------------------------------\n");
  // for(int i=0; i<100; ++i){
  //   if(childPIDs[i] != -1){
  //     printf("CPID: %d\n", childPIDs[i]);
  //   }
  // }
  // printf("--------------------------------------\n"); 

  printf("------------------------------------\n");
  for(int i=0; i<100; ++i){
    if(childPIDs[i]!=-1){
      waitpid(childPIDs[i], &status, WNOHANG);
      if(status != -1){
        printf("Child with pid %d exited\n", childPIDs[i]);
        status = -1;
        childPIDs[-1] = -1;
      }
    }
  }
  printf("------------------------------------\n");

  // printf("####################################################################\n");
  // printf("Status value: %d\n", status);
  // printf("####################################################################\n");
}

int main(int argc, char **argv){
  signal(SIGCHLD, handler);
  for(int i=0; i<100; ++i){
    childPIDs[i] = -1;
  }

  printf("Parent PID: %d\n", getpid());
  int connfd, lfd, listenfd, i = 0;
  pid_t pid, ret;
  // int p[2];
  FILE *fp;
  socklen_t clilen;
  char buff[20000], *buf, ch;
  struct sockaddr_in cliaddr, servaddr;
  
  if(argc != 2){
    fprintf(stderr, "Usage %s <Server Port(Integer)>\n", argv[0]);
    exit(1);
  }
  if((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror ("sockfd error");
    exit(1);
  }

  buf = "HTTP/1.1 200 OK\nDate: Mon, 19 Oct 2009 01:26:17 GMT\nServer: Apache/1.2.6 Red Hat\nContent-Length: 18\nAccept-Ranges: bytes\nKeep-Alive: timeout=15, max=100\nConnection: Keep-Alive\nContent-Type: text/html\n\n<html>Hello</html>";

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));
  
  if(bind(lfd, (struct sockaddr *) &servaddr, sizeof (servaddr))){
    perror("Bind error");
    exit(1);
  }
  listen(lfd, 10);
  
  for(;;){
    clilen = sizeof(cliaddr);
    
    if((connfd = accept(lfd, (struct sockaddr *) &cliaddr, &clilen)) < 0){
      perror("connection error");
      exit(1);
    }
    
    if((pid = fork ()) < 0){
      perror("fork:");
      exit(1);
    }
    if(pid == 0){
      printf("----------------------------------------------\n");
      printf("Child PID: %d\n", getpid());
      printf("----------------------------------------------\n");
      close(listenfd);
      recv(connfd, buff, sizeof (buff), 0);
      printf("%s\n", buff);
      send(connfd, buf, strlen (buf), 0);
      close(connfd);
      // sleep(100);
      exit(0);
    }

    childPIDs[ind++] = pid;
    // wait(NULL); //Calling wait will turn this concurrent server into an iterative server
    close(connfd);
  }
}