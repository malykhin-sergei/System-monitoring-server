#ifndef MAIN_H_
#define MAIN_H_
#define _POSIX_C_SOURCE 200112L
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

char *system_state_report();

/* Thread routine to observe system state */
void *pthread_sysinfo ();

/* Thread routine to serve TCP connection to client. */
void *pthread_routine_tcp (void *arg);

int udp_reply (const int socket_fd);

void signal_handler (int signal_number);

#endif // MAIN_H_
