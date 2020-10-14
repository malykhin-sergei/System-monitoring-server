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
#include <netdb.h>
#include <unistd.h>
#include "main.h"

#define BUF_SIZE 500

pthread_rwlock_t rwlock;

typedef struct pthread_arg_t
{
    int new_socket_fd, socket_fd;
    struct sockaddr_in client_address;
} pthread_arg_t;

int connections = 0;
char system_state[4096];
enum { TCP, UDP } protocol;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf ("Usage: %s <TCP/UDP> <port> <max_connections>\n", argv[0]);
        return 0;
    }

    if (strncmp ("TCP", argv[1], 3) == 0)
    {
        printf ("Using TCP");
        protocol = TCP;
    }
    else if (strncmp ("UDP", argv[1], 3) == 0)
    {
        printf ("Using UDP");
        protocol = UDP;
    }
    else
    {
        printf ("Unknown protocol: %s\n", argv[1]);
        printf ("Usage: %s <TCP/UDP> <port> <max_connections>\n", argv[0]);
        return 0;
    }

    const int port = atoi (argv[2]);

    if (!port)
    {
        printf ("Wrong port number: %s\n", argv[2]);
        printf ("Usage: %s <TCP/UDP> <port> <max_connections>\n", argv[0]);
        return 0;
    }

    const int max_connections = atoi (argv[3]);

    if (!max_connections)
    {
        printf ("Wrong max_connections number: %s\n", argv[3]);
        printf ("Usage: %s <TCP/UDP> <port> <max_connections>\n", argv[0]);
        return 0;
    }

    printf (" on port %i with no more than %i clients\n", port, max_connections);

    /* Assign signal handlers to signals. */

    if (signal (SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        perror ("signal");
        exit (EXIT_FAILURE);
    }
    if (signal (SIGTERM, signal_handler) == SIG_ERR)
    {
        perror ("signal");
        exit (EXIT_FAILURE);
    }
    if (signal (SIGINT, signal_handler) == SIG_ERR)
    {
        perror ("signal");
        exit (EXIT_FAILURE);
    }

    pthread_attr_t pthread_attr;
    pthread_arg_t *pthread_arg;
    pthread_t pthread;

    if (pthread_attr_init (&pthread_attr) != 0)
    {
        perror("pthread_attr_init");
        exit (EXIT_FAILURE);
    }
    if (pthread_attr_setdetachstate (&pthread_attr, PTHREAD_CREATE_DETACHED) != 0)
    {
        perror("pthread_attr_setdetachstate");
        exit (EXIT_FAILURE);
    }

    // start observation

    pthread_rwlock_init (&rwlock, NULL);
    if (pthread_create (&pthread, &pthread_attr, pthread_sysinfo, NULL) != 0)
    {
        perror("pthread_create");
        exit (EXIT_FAILURE);
    }

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int socket_fd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = (protocol == TCP) ? SOCK_STREAM : SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    int s = getaddrinfo(NULL, argv[2], &hints, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        socket_fd = socket(rp->ai_family, rp->ai_socktype,
                           rp->ai_protocol);
        if (socket_fd == -1)
            continue;

        if (bind(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

        close (socket_fd);
    }

    if (rp == NULL)                 /* No address succeeded */
    {
        fprintf(stderr, "Could not bind\n");
        exit (EXIT_FAILURE);
    }

    freeaddrinfo (result);

    if (protocol == UDP)
    {
        struct timeval timeout = {5, 0};
        setsockopt (socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));

        for (;; udp_reply (socket_fd));
    }

    if (listen (socket_fd, BACKLOG) == -1)
    {
        perror ("listen");
        exit (EXIT_FAILURE);
    }

    while (protocol == TCP)
    {
        pthread_arg = (pthread_arg_t *) malloc (sizeof *pthread_arg);
        if (!pthread_arg)
        {
            perror ("malloc");
            exit (EXIT_FAILURE);
        }

        socklen_t client_address_len = sizeof pthread_arg->client_address;
        int tcp_socket_fd = accept (socket_fd, (struct sockaddr *)&pthread_arg->client_address,
                                    &client_address_len);

        connections++;

        if (tcp_socket_fd == -1)
        {
            perror ("accept");
            free (pthread_arg);
            exit (EXIT_FAILURE);
        }
        else if (connections > max_connections)
        {
            close (tcp_socket_fd);
            connections--;
            free (pthread_arg);
            continue;
        }

        printf ("New TCP connection accepted: now there are %i clients\n", connections);
        pthread_arg->new_socket_fd = tcp_socket_fd;
        if (pthread_create (&pthread, &pthread_attr, pthread_routine_tcp, (void *)pthread_arg) != 0)
        {
            perror("pthread_create");
            free (pthread_arg);
            exit (EXIT_FAILURE);
        }
    }
    return 0;
}

void *pthread_sysinfo ()
{
    char *s = system_state_report ();
    strcpy (system_state, s);
    free (s);

    for (;;)
    {
        if (connections > 0 || protocol == UDP)
        {
            s = system_state_report ();
            pthread_rwlock_wrlock (&rwlock);
            strcpy (system_state, s);
            pthread_rwlock_unlock (&rwlock);
            free (s);
        }
    }
    return NULL;
}

void signal_handler (int signal_number)
{
    /* Exit cleanup code here. */
    // close (socket_fd);
    exit (EXIT_SUCCESS);
}
