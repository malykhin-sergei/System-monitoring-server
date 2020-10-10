#include "main.h"

extern char system_state[4096];
sem_t empty, full;

int udp_reply (const int socket_fd, struct sockaddr_in client_address)
{
    socklen_t len = sizeof (client_address);
    char msg_buffer[80];
    bzero (msg_buffer, 80);

    int client_response = recvfrom (socket_fd, msg_buffer, sizeof (msg_buffer), 0,
                                    (struct sockaddr*)&client_address, &len);

    if (client_response == 0)
        return 0;

    if (strncmp ("report", msg_buffer, 6) == 0)
    {
        sem_wait (&full);
        sendto (socket_fd, system_state, strlen (system_state) + 1, 0,
                (struct sockaddr*)&client_address, len);
        sem_post (&empty);
//        printf ("%s\n", system_state);
    }

    return 0;
}
