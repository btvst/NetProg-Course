#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>

#define MSGSIZE 16

/* apply O_NONBLOCK falg to the fifo on reading */

/* Unlike Pipe a FIFO can't be used to read and write from the same end */

/* Writes of more than PIPE_BUF are nonatomic */

int main(){
	int i;
	char *msg = "How are you?";
	char inbuff[MSGSIZE];

	int rfd, wfd;
	if(mkfifo("fifo", O_CREAT | O_EXCL | 0666) < 0){
		if(errno == EEXIST){
			perror("fifo");
		}
	}

	pid_t ret;
	ret = fork();

	if(ret > 0){
		rfd = open("fifo", O_RDONLY | O_NONBLOCK); //prevent blocking
		wfd = open("fifo", O_WRONLY);

		i=0;
		while(i < 10){
			write(wfd, msg, MSGSIZE);
			sleep(2);
			read(rfd, inbuff, MSGSIZE);
			printf("child: %s\n", inbuff);
			++i;
		}

		exit(1);
	}
	else{
		rfd = open("fifo", O_RDONLY | O_NONBLOCK);
		wfd = open("fifo", O_WRONLY);
		i=0;
		while(i < 10){
			sleep(1);
			read(rfd, inbuff, MSGSIZE);
			printf("parent: %s\n", inbuff);
			write(wfd, "i am fine", strlen("i am fine"));
			++i;
		}
		exit(0);
	}
}