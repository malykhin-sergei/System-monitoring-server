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
#include "main.h"

typedef struct pthread_arg_t
{
    int new_socket_fd, socket_fd;
    struct sockaddr_in client_address;
} pthread_arg_t;

extern int connections;
extern char system_state[4096];
extern pthread_rwlock_t rwlock;

void *pthread_routine_tcp (void *arg)
{
    pthread_arg_t *pthread_arg = (pthread_arg_t *) arg;
    int tcp_socket_fd = pthread_arg->new_socket_fd;
//   struct sockaddr_in client_address = pthread_arg->client_address;

    free (arg);

    char msg_buffer[80];

    for (;;)
    {
        bzero (msg_buffer, 80);

        int client_response = read (tcp_socket_fd, msg_buffer, sizeof (msg_buffer));
        if (client_response == 0)
            break;

        if (strncmp ("stop", msg_buffer, 4) == 0)
            break;

        if (strncmp ("report", msg_buffer, 6) == 0)
        {
            pthread_rwlock_rdlock (&rwlock);
            write (tcp_socket_fd, system_state, strlen(system_state) + 1);
            pthread_rwlock_unlock (&rwlock);
        }
    }

    close (tcp_socket_fd);
    printf ("Client %i stops observation\n", connections--);
    return NULL;
}

