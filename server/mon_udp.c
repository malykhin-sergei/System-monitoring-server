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

extern char system_state[4096];
extern pthread_rwlock_t rwlock;

int udp_reply (const int socket_fd)
{
    struct sockaddr_in client_address;
    socklen_t len = sizeof (client_address);
    char msg_buffer[80];
    bzero (msg_buffer, 80);

    int client_response = recvfrom (socket_fd, msg_buffer, sizeof (msg_buffer), 0,
                                    (struct sockaddr*)&client_address, &len);

    if (client_response == 0)
        return 0;

    if (strncmp ("report", msg_buffer, 6) == 0)
    {
//        char *msg = system_state_report ();
//        sendto (socket_fd, msg, strlen (msg) + 1, 0, (struct sockaddr*)&client_address, len);
        pthread_rwlock_rdlock (&rwlock);
        sendto (socket_fd, system_state, strlen (system_state) + 1, 
            0, (struct sockaddr*)&client_address, len);
        pthread_rwlock_unlock (&rwlock);
    }

    return 0;
}
