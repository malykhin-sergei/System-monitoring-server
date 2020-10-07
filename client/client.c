#define _POSIX_C_SOURCE 200112L

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>

#define BUF_SIZE 500

// 127.0.0.1 27015 1

int status (const char *src, char *report);

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;

    if (argc < 4)
    {
        fprintf (stderr, "Usage: %s host port time_lag ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int time_lag = atoi (argv[3]);

    if (!time_lag)
    {
        fprintf( stderr, "Impossible time lag: %s\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* Allow IPv4 */
    hints.ai_socktype = 0;           /* Any type: TCP/UDP */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;           /* Any protocol */

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(sfd);
    }

    if (rp == NULL)
    {
        fprintf (stderr, "Could not connect to server %s at port: %s\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    // Server interaction.

    for (;; sleep (time_lag))
    {
        char msg[4096];
        char s[256];
        bzero (msg, 4096);

        write (sfd, "report", 6);
        int server_response = read (sfd, msg, 4096);
        if (server_response <= 0)
        {
            printf ("Connection is closed by server\n");
            break;
        }
        status (msg, s);
        printf ("%s\n", s);
    }

    exit(EXIT_SUCCESS);
}
