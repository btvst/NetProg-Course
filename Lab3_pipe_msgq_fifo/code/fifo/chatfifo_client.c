// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <sys/stat.h>
// #include <errno.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <string.h>
// #include <signal.h>

// typedef struct{
//   int dpid;
//   int spid;
//   char chat[100];
// }Message;

// Message incoming, outgoing;

// void handler(){

// }

// int main(){
//   signal(SIGUSR1, handler);
//   int i, fd, fd1, n, dummyfd;
//   char buf[100];
//   char myfifoname[30], fifonameserver[30];
//   // Message incoming, outgoing;

//   printf("My pid is %d\n", getpid());

//   sprintf(myfifoname, "/tmp/%d", getpid());
//   i = mkfifo(myfifoname, 0666); //Make FIFO named specifically with the PID of the current process
//   if(i < 0){
//     if(errno == EEXIST){
//       printf("errno is set as %d\n", errno);
//     }
//     perror("mkfifo");
//   }

//   //sprintf(fifonameserver,"/tmp/server.netproglab4.%s",getenv("LOGNAME"));
//   dummyfd = open("/tmp/chatfifoserver", O_RDONLY | O_NONBLOCK);
//   fd1 = open("/tmp/chatfifoserver", O_WRONLY);

//   outgoing.spid=getpid();
//   printf("Enter destination pid and the message: \n");
//   scanf("%d\n", &outgoing.dpid);
//   fflush(stdout);
//   fflush(stdin);

//   fgets(buf, 100, stdin);
//   strcpy(outgoing.chat, buf);

//   write(fd1, &outgoing, sizeof(Message));
//   printf("Message sent to %d. Waiting for reply...\n", outgoing.dpid);

//   sleep(0.5);
//   fd = open(myfifoname, O_RDONLY);
//   int op1=0;

//   while((n = read(fd, &incoming, sizeof(Message))) > 0){
//     printf ("\nSender:%d, Message:%s\n", incoming.spid,incoming.chat);
//     printf("Send reply (1/0):\n");
//     fscanf(stdin,"%d\n",&op1);
//     fflush(stdin);
//     fflush(stdout);
//     if(op1!=0){
//       printf("Enter the reply:\n");
//       fflush(stdout);
//       fgets(buf,100,stdin);
    
//       outgoing.spid=getpid();
//       outgoing.dpid=incoming.spid;
      
//       strcpy(outgoing.chat,buf);
//       write(fd1, &outgoing, sizeof(Message));
      
//       printf("Message sent to %d. Waiting for reply...\n", outgoing.dpid);
//     }
//   }

//   unlink (myfifoname);
// }

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

typedef struct{
  int dpid;
  int spid;
  char chat[100];
}Message;

int i, fd, fd1, n, dummyfd;
char buf[100];
char myfifoname[30], fifonameserver[30];
Message incoming, outgoing;

void handler(){
  int n;
  if(n = read(fd, &incoming, sizeof(Message)) > 0){
    printf ("\nSender:%d, Message:%s\n", incoming.spid,incoming.chat);
  }
}

int main(){
  signal(SIGUSR1, handler);
  // int i, fd, fd1, n, dummyfd;
  // char buf[100];
  // char myfifoname[30], fifonameserver[30];
  // Message incoming, outgoing;

  printf("My pid is %d\n", getpid());

  sprintf(myfifoname, "/tmp/%d", getpid());
  i = mkfifo(myfifoname, 0666); //Make FIFO named specifically with the PID of the current process
  if(i < 0){
    if(errno == EEXIST){
      printf("errno is set as %d\n", errno);
    }
    perror("mkfifo");
  }

  //sprintf(fifonameserver,"/tmp/server.netproglab4.%s",getenv("LOGNAME"));
  dummyfd = open("/tmp/chatfifoserver", O_RDONLY | O_NONBLOCK);
  fd1 = open("/tmp/chatfifoserver", O_WRONLY);

  outgoing.spid=getpid();
  printf("Enter destination pid and the message: \n");
  scanf("%d\n", &outgoing.dpid);
  fflush(stdout);
  fflush(stdin);

  fgets(buf, 100, stdin);
  strcpy(outgoing.chat, buf);

  write(fd1, &outgoing, sizeof(Message));
  printf("Message sent to %d. Waiting for reply...\n", outgoing.dpid);

  sleep(0.5);
  fd = open(myfifoname, O_RDONLY);
  int op1=0;

  while(1){
    // printf ("\nSender:%d, Message:%s\n", incoming.spid,incoming.chat);
    sleep(0.5);
    printf("Send reply (1/0):\n");
    fscanf(stdin,"%d\n",&op1);
    // fflush(stdin);
    // fflush(stdout);
    if(op1!=0){
      printf("Enter the reply:\n");
      fflush(stdout);
      fgets(buf,100,stdin);
    
      outgoing.spid=getpid();
      outgoing.dpid=incoming.spid;
      
      strcpy(outgoing.chat,buf);
      write(fd1, &outgoing, sizeof(Message));
      
      printf("Message sent to %d. Waiting for reply...\n", outgoing.dpid);
    }
  }

  unlink (myfifoname);
}