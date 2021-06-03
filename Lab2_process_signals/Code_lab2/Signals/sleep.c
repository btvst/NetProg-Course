 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

void mysleepUtil(long n){
    alarm(n);
    pause(); //pauses the process until some signal either terminating a thread or executing a signal handler is delivered
}

void alarm_handler(int signo){
    printf("Alarm\n");
}

int main(){
    signal(SIGALRM, alarm_handler);

    struct timeval t1, t2;
    double elapsedTime;

    gettimeofday(&t1, NULL);
    mysleepUtil(4);
    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec - t1.tv_sec)*1000;
    elapsedTime += (t2.tv_usec - t1.tv_usec)/1000;
    printf("Time: %lf\n",elapsedTime);
}