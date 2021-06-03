/* OVERVIEW -
 *
 * N child processes are created, N taken as argument.
 * 
 * Parent first accepts, connection call from all the child processes before performing further.
 * 
 * Each child process creates 10/N, 100/N, 1000/N TCP connections or socket FDs. This is to prevent
 * utilization of all sockets in case of N is large. Ex- If N = 10, N*1000 = 10000 (not possible)
 * 
 * Child sends its PID, time of sending the message randomly to any of the 10/N, 100/N, 1000/N sockets in the form of struct pack.
 * 
 * Parent receives the child packet and prints the time delay between the message is sent and the return of select/epoll system call.
 * 
 * Performance is measured only for data sent, not for listening socket and acceptance using epoll and select.
*/

/* USAGE -
 * 
 * $ make run
 * $ ./pro
 * 
 * Enter the number of child processes to be created as an argument.
 * Enter the number of messages each child should send before terminating.
 * 
 * If N > no. of FDs (say 10), then this case is not executed, as it is impossible to divide the FDs equally between the child processes.
*/

/* OUTPUT FORMAT -
 * The program outputs, the time delay between the point a message is sent, and between the point the select/epoll call
 * detects the change of state.
 *
 * For each message received from child using SELECT in parent
 * SELECT -- Child PID: %d -- Time in ms(for the particular select to end wait): %lf -- No. of descriptors monitored: %d
 * 
 * For each message received from child using EPOLL in parent
 * EPOLL -- Child PID: %d -- Time in ms(for the particular select to end wait): %lf -- No. of descriptors monitored: %d
 * 
 * Final statictics calculated show the average response time for SELECT and EPOLL for all the messages received
 * respectively while 10, 100 and 1000 FDs were being monitored. 
 * This is calculated by adding the elapsed time for all messages and dividing 
 * by count of total messages = N * NO_OF_MESSAGES (sent by 1 child). 
 * This should be taken as final comparison parameter, as individual SELECT and EPOLL response time 
 * might not show true comparison.
 * 
 * It might take few seconds before the output shows, as tge program sleeps few times in between execution to prevent
 * erraneous results.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h> // struct sockaddr_in
#include <sys/types.h>
#include <arpa/inet.h> // inet_ntoa
#include <time.h> // time() function
#include <strings.h> // bzero function
#include <sys/time.h> // gettimeofday()
#include <limits.h> // INT_MAX and INT_MIN
#include <sys/epoll.h> // epoll()

#define PORT 1254

int NO_OF_MESSAGES;

typedef struct packet{
    pid_t c_pid;
    struct timeval send_time;
    int no_of_conn;
} pack;

void print_address(struct sockaddr_in *addr){
    printf("----------------------------------------\n");
    printf("Family: %d\n", addr->sin_family);
    printf("IP: %s\n", inet_ntoa(addr->sin_addr));
    printf("Port: %d\n", ntohs(addr->sin_port));
    printf("----------------------------------------\n");
}

void die_with_error(char *msg){
    perror(msg);
    exit(1);
}

void cleanup(int *children, int *status, int N, int *fds, int total_fds, int parent_sockfd){
    for(int i=0; i<N; ++i){
        waitpid(children[i], &status[i], 0);
    }

    for(int i=0; i<total_fds; ++i){
        close(fds[i]);
    }

    close(parent_sockfd);
}

void child_work(pid_t child, pid_t parent, int N, int monitor, struct sockaddr_in parent_addr){
    // sleep(2); // sleep to allow parent to fork all child processes first
    // connect is blocking, hence no need

    pack p;
    int lim = monitor/N;
    p.no_of_conn = lim;
    p.c_pid = getpid();
    int num = 0;
    int n;

    int sockfd[lim];
    srandom(getpid());

    for(int i=0; i<lim; ++i){
        if((sockfd[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
            die_with_error("socket() in child");
        }
    }

    for(int i=0; i<lim; ++i){
        if(connect(sockfd[i], (struct sockaddr *)&parent_addr, sizeof(struct sockaddr)) == -1){
            die_with_error("connect() in child");
        }
    }

    /* 
     * sleep for some time before randomly sending data along the sockets
     * to allow the parent to first accept all the child sockets, otherwise 
     * messages from this child might be sent before the parent, has accepted 
     * the connection requests from other children.
     * 
     * This will cause, delay between the time message was sent, 
     * and select was waiting, giving erraneous results.
    */

    if(monitor == 10){
        sleep(2);
    }
    else if(monitor == 100){
        sleep(4);
    }
    else if(monitor == 1000){
        sleep(8);
    }

    while(num < NO_OF_MESSAGES){
        // int sl = (random()%5)+1;
        // sleep(sl);

        int ind = random()%lim;
        // printf("%d message sent from child %d to sockfd: %d\n", num+1, p.c_pid, sockfd[ind]);

        gettimeofday(&p.send_time, NULL); //store sending time
        n = write(sockfd[ind], &p, sizeof(pack));

        // sleep(1);

        ++num;
    }

    for(int i=0; i<sockfd[lim]; ++i){
        close(sockfd[i]);
    }
}

int prepare_listening_socket_ipv4(struct sockaddr_in *parent_addr, char *ip, int port, int max_pending){
    bzero(parent_addr, sizeof(struct sockaddr));
    parent_addr->sin_family = AF_INET;
    parent_addr->sin_addr.s_addr = inet_addr(ip);
    parent_addr->sin_port = htons(port);

    int parent_sockfd;

    if((parent_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        die_with_error("socket() failed in parent");
    }
    printf("Parent socket created\n");

    if(bind(parent_sockfd, (struct sockaddr *)parent_addr, sizeof(struct sockaddr)) == -1){
        die_with_error("bind() failed in parent");
    }
    printf("Parent socket bound to address\n");
    print_address(parent_addr);

    if(listen(parent_sockfd, max_pending) == -1){
        die_with_error("listen() failed");
    }
    printf("Parent listening\n");

    return parent_sockfd;
}

double run_select(int N, int monitor, int port){
    if(N > monitor){
        printf("%d descriptors can't be divided into %d processes\n", monitor, N);
        return 0.0;
    }

    int parent_sockfd, max=0, total_fds, max_pending = N, ct_10 = 0, ct_100 = 0, ct_1000 = 0;
    fd_set rset;
    struct sockaddr_in parent_addr, child_addr;
    socklen_t socket_len;
    pack p;
    pid_t children[N];
    int status[N];
    double sum_10 = 0.0, sum_100 = 0.0 , sum_1000 = 0.0;

    // Prepare TCP parent server process
    parent_sockfd = prepare_listening_socket_ipv4(&parent_addr, "0.0.0.0", port, max_pending);
    
    // Fork parent to produce child client process

    for(int i=0; i<N; ++i){
        if((children[i] = fork()) < 0){
            die_with_error("fork() failed");
        }
        else if(children[i] == 0){
            child_work(children[i], getppid(), N, monitor, parent_addr);
            exit(0);
        }
    }

    total_fds = ((int)(monitor/N))*N; // No. of fds that will be created for N child processes
    int fds[total_fds];

    for(int i=0; i<total_fds; ++i){
        bzero(&child_addr, sizeof(struct sockaddr_in));
        socket_len = sizeof(struct sockaddr);
        
        if((fds[i] = accept(parent_sockfd, (struct sockaddr *)&child_addr, &socket_len)) == -1){
            die_with_error("accept() failed in parent");
        }

        // printf("fds[i]: %d\n", fds[i]);
        if(fds[i] > max){
            max = fds[i];
        }
    }

    // printf("max: %d ; monitor: %d\n", max, monitor);

    int ct = 0;
    while(ct < N*NO_OF_MESSAGES){ //If the no. of data packets received is equal to the expected no. of total data packets, exit the loop, might get stuck in an infinite loop. if any data packet is lost.

        FD_ZERO(&rset);
        for(int i=0; i<total_fds; ++i){
            FD_SET(fds[i], &rset);
        }

        // printf("Going into select\n");
        select(max+1, &rset, NULL, NULL, NULL);

        struct timeval time_after_select;
        gettimeofday(&time_after_select, NULL);

        for(int i=0; i<total_fds; ++i){
            if(FD_ISSET(fds[i], &rset)){
                bzero(&p, sizeof(pack));
                // printf("set fds[i]: %d\n", fds[i]);

                if(read(fds[i], &p, sizeof(pack)) != 0 && p.c_pid != 0){ // do not calculate time for FIN segments

                    //Calculation of elapsed time
                    double elapsed_time;
                    elapsed_time = (time_after_select.tv_sec - p.send_time.tv_sec) * 1000.0;
                    elapsed_time += (time_after_select.tv_usec - p.send_time.tv_usec)/1000.0;
                    printf("SELECT -- Child PID: %d -- Response time in ms: %lf -- No. of monitored descriptors: %d\n", p.c_pid, elapsed_time, monitor);

                    //Calculation of avg elapsed time
                    if(monitor == 10){
                        sum_10 += elapsed_time;
                        ++ct_10;
                    }
                    else if(monitor == 100){
                        sum_100 += elapsed_time;
                        ++ct_100;
                    }
                    else if(monitor == 1000){
                        sum_1000 += elapsed_time;
                        ++ct_1000;
                    }

                    ++ct; // Increase count of received messages only for data messages, not FIN and RST
                }
            }
        }

    }

    cleanup(children, status, N, fds, total_fds, parent_sockfd);

    // Show total count of data messages received

    if(monitor == 10){
        printf("Count of messages received (should be = N*NO_OF_MESSAGES): %d\n", ct_10);
        return (double)sum_10/(double)ct_10;
    }
    else if(monitor== 100){
        printf("Count of messages received (should be = N*NO_OF_MESSAGES): %d\n", ct_100);
        return (double)sum_100/(double)ct_100;
    }
    else if(monitor == 1000){
        printf("Count of messages received (should be = N*NO_OF_MESSAGES): %d\n", ct_1000);
        return (double)sum_1000/(double)ct_1000;
    }

    return 0.0;
}

double run_epoll(int N, int monitor, int port){
    if(N > monitor){
        printf("%d descriptors can't be divided into %d processes\n", monitor, N);
        return 0.0;
    }

    int parent_sockfd, max=0, ct_10 = 0, ct_100 = 0, ct_1000 = 0;
    int max_pending = N;
    struct sockaddr_in parent_addr, child_addr;
    socklen_t socket_len;
    int children[N];
    int status[N];
    double sum_10 = 0.0, sum_100 = 0.0, sum_1000 = 0.0;

    // Prepare TCP parent server process

    parent_sockfd = prepare_listening_socket_ipv4(&parent_addr, "0.0.0.0", port, max_pending);

    // Fork to get children
    for(int i=0; i<N; ++i){
        if((children[i]=fork()) == 0){
            child_work(getpid(), getppid(), N, monitor, parent_addr);
            exit(0);
        }
        else if(children[i] < 0){
            die_with_error("fork() failed in run_epoll");
        }
    }

    int total_fds = ((int)(monitor/N))*N;
    int fds[total_fds];

    // Accept connection requests from child processes
    int epoll_fd = epoll_create(total_fds);
    if(epoll_fd == -1){
        die_with_error("epoll() failed in run_epoll");
    }

    for(int i=0; i<total_fds; ++i){
        static struct epoll_event ev1;
        bzero(&child_addr, sizeof(struct sockaddr_in));
        socket_len = sizeof(struct sockaddr);
        
        if((ev1.data.fd = accept(parent_sockfd, (struct sockaddr *)&child_addr, &socket_len)) == -1){
            die_with_error("accept() failed in parent");
        }
        ev1.events = EPOLLIN;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev1.data.fd, &ev1);
        // printf("FD: %d\n", ev1.data.fd);

        fds[i] = ev1.data.fd;
    }

    struct epoll_event events[total_fds];

    // wait for data
    int ct = 0;
    int nfds;
    pack p;
    while(1){
        nfds = epoll_wait(epoll_fd, events, total_fds, -1);

        // printf("NFDS: %d\n", nfds);
        struct timeval time_after_epoll;
        gettimeofday(&time_after_epoll, NULL);

        if(nfds == -1){
            die_with_error("epoll_wait() failed");
        }

        for(int i=0; i<nfds; ++i){
            bzero(&p, sizeof(pack));
            // printf("set fds[i]: %d\n", fds[i]);

            if(read(events[i].data.fd, &p, sizeof(pack)) != 0 && p.c_pid != 0){ // do not calculate time for FIN segments

                double elapsed_time;
                elapsed_time = (time_after_epoll.tv_sec - p.send_time.tv_sec) * 1000.0;
                elapsed_time += (time_after_epoll.tv_usec - p.send_time.tv_usec)/1000.0;
                printf("EPOLL -- Child PID: %d -- Response time in ms: %lf -- No. of monitored descriptos: %d\n", p.c_pid, elapsed_time, monitor);

                // Calculate avg elapsed time
                if(monitor == 10){
                    sum_10 += elapsed_time;
                    ++ct_10;
                }
                else if(monitor == 100){
                    sum_100 += elapsed_time;
                    ++ct_100;
                }
                else if(monitor == 1000){
                    sum_1000 += elapsed_time;
                    ++ct_1000;
                }
                ++ct;
            }
        }

        if(ct >= N*NO_OF_MESSAGES){ //If the no. of data packets received is equal to the expected no. of total data packets, exit the loop, might get stuck in an infinite loop. if any data packet is lost.
            break;
        }
    }

    cleanup(children, status, N, fds, total_fds, parent_sockfd);

    if(monitor == 10){
        printf("Count of messages received (should be = N*NO_OF_MESSAGES): %d\n", ct_10);
        return (double)sum_10/(double)ct_10;
    }
    else if(monitor == 100){
        printf("Count of messages received (should be = N*NO_OF_MESSAGES): %d\n", ct_100);
        return (double)sum_100/(double)ct_100;
    }
    else if(monitor == 1000){
        printf("Count of messages received (should be = N*NO_OF_MESSAGES): %d\n", ct_1000);
        return (double)sum_1000/(double)ct_1000;
    }

    return 0.0;
}

int main(int argc, char *argv[]){
    int N;
    printf("Enter N(no. of child processes): ");
    scanf("%d", &N);
    printf("Enter no. of messages each child should randomly send (Enter 1 if only a single message sent by 1 child): ");
    scanf("%d", &NO_OF_MESSAGES);

    double avg_10, avg_100, avg_1000;
    double epoll_avg_10, epoll_avg_100, epoll_avg_1000;

    avg_10 = run_select(N, 10, PORT);
    avg_100 = run_select(N, 100, PORT+1);
    avg_1000 = run_select(N, 1000, PORT+2);

    epoll_avg_10 = run_epoll(N, 10, PORT+3);
    epoll_avg_100 = run_epoll(N, 100, PORT+4);
    epoll_avg_1000 = run_epoll(N, 1000, PORT+5);

    printf("------------------------------------SELECT Final statistics-----------------------------------------\n");
    printf("Tot. response time for 10 FDs: %lf\n", (avg_10)*(N*NO_OF_MESSAGES));
    printf("Tot. response time for 100 FDs: %lf\n", (avg_100)*(N*NO_OF_MESSAGES));
    printf("Tot. response time for 1000 FDs: %lf\n", (avg_1000)*(N*NO_OF_MESSAGES));
    printf("Avg. response time for 10 FDs: %lf\n", (avg_10));
    printf("Avg. response time for 100 FDs: %lf\n", (avg_100));
    printf("Avg. response time for 1000 FDs: %lf\n", (avg_1000));
    printf("----------------------------------------------------------------------------------------------------\n");

    printf("-------------------------------------EPOLL Final statistics-----------------------------------------\n");
    printf("Tot. response time for 10 FDs: %lf\n", (epoll_avg_10)*(N*NO_OF_MESSAGES));
    printf("Tot. response time for 100 FDs: %lf\n", (epoll_avg_100)*(N*NO_OF_MESSAGES));
    printf("Tot. response time for 1000 FDs: %lf\n", (epoll_avg_1000)*(N*NO_OF_MESSAGES));
    printf("Avg. response time for 10 FDs: %lf\n", (epoll_avg_10));
    printf("Avg. response time for 100 FDs: %lf\n", (epoll_avg_100));
    printf("Avg. response time for 1000 FDs: %lf\n", (epoll_avg_1000));
    printf("----------------------------------------------------------------------------------------------------\n");
}