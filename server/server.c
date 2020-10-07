#include "main.h"

int connections = 0;

int main(int argc, char *argv[])
{
    struct timeval timeout = {5, 0};
    struct sockaddr_in server_address, client_address;

    pthread_attr_t pthread_attr;
    pthread_arg_t *pthread_arg;
    pthread_t pthread;

    // Parse command line arguments

    if (argc != 4)
    {
        printf ("Usage: %s <TCP/UDP> <port> <max_connections>\n", argv[0]);
        return 0;
    }

    enum protocol_type protocol;

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

    /* Initialise pthread attribute to create detached threads. */
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

    if (pthread_create (&pthread, &pthread_attr, pthread_sysinfo, NULL) != 0)
    {
        perror("pthread_create");
        exit (EXIT_FAILURE);
    }

    /* Initialise IPv4 address. */
    memset (&server_address, 0, sizeof server_address);
    memset (&client_address, 0, sizeof client_address);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons (port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Create UDP/TCP socket.

    int socket_fd;

    if (protocol == TCP)
        socket_fd = socket (AF_INET, SOCK_STREAM, 0);
    else if (protocol == UDP)
        socket_fd = socket (AF_INET, SOCK_DGRAM, 0);

    if (socket_fd == -1)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    /* Bind address to socket. */
    if (bind (socket_fd, (struct sockaddr *)&server_address, sizeof server_address) == -1)
    {
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    if (protocol == UDP)
    {
        setsockopt (socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));
        for (;; udp_reply (socket_fd, client_address));
    }

    if (listen (socket_fd, BACKLOG) == -1)
    {
        perror ("listen");
        exit (EXIT_FAILURE);
    }

    while (protocol == TCP)
    {
        /* Create pthread argument for each connection to client. */
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

        /* Create thread to serve connection to client. */
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
    for (;; system_state = system_state_report ());
    return NULL;
}

void signal_handler (int signal_number)
{
    /* Exit cleanup code here. */
    // close (socket_fd);
    exit (EXIT_SUCCESS);
}
