#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

void seg_fault_handler(int signo){
    printf("SEGMENTATION FAULT caught\n");
    sleep(1);
}

int main(){
    signal(SIGSEGV, seg_fault_handler);

    int n;
    printf("Enter a number:\n"); 
    // SIGSEGV is generated continuosly as the program execution returns back here only
    
    scanf("%d",n);

    printf("After segmentation fault\n");
}