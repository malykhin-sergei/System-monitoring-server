#include "main.h"

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
        sendto (socket_fd, system_state, strlen(system_state) + 1, 0,
                (struct sockaddr*)&client_address, len);
    }

    return 0;
}
