#ifndef MAIN_H_
#define MAIN_H_

#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

#define BACKLOG 50

typedef struct pthread_arg_t
{
    int new_socket_fd, socket_fd;
    struct sockaddr_in client_address;
} pthread_arg_t;

char *system_state_report();

enum protocol_type { TCP, UDP };

/* Thread routine to observe system state */
void *pthread_sysinfo ();

/* Thread routine to serve TCP connection to client. */
void *pthread_routine_tcp (void *arg);

int udp_reply (const int socket_fd, struct sockaddr_in client_address);

/* Signal handler to handle SIGTERM and SIGINT signals. */
void signal_handler (int signal_number);

#endif // MAIN_H_
