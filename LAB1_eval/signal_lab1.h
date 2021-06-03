#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

/* N processes are created apart from the main function. Each of these N, generate Z%13 processes, where
 * Z is the PID of the process */

/* Each process sends SIGUSR1 signal to i+1, i+2 ..., i+12 processes 10 times each after 2 seconds */

/* After 20s, each process prints its SIGUSR1 count, if 0, exit there only, else exit normally */

/* Due to crash, I have run the whole code once only */

extern int sigusr1;

/* Function to generate alarm signal every 2 seconds to calling process */
void receiveTimerSignal();

/* Signal handler for SIGALRM to send SIGUSR1 signal to existing i+1, i+2, ..., i+12 process */ 
void handle_alarm();

/* Signal handler to count number of SIGUSR1 signals received by a process */
void counter(int signo);

/* Function to check existing process from (i+1, i+2, ..., i+12) + pid for current process */
void checkNext12();

/* Process generation function to create pid%13 processes for all the n processes */
void createZProcesses(int z);

/* Process generation function to create n processes, where n is an argument inputted from user */
void createProcesses(int n);
