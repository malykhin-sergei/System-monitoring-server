#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <netdb.h>
#include <ifaddrs.h>
#include <jansson.h>

int net_usage (json_t *net_state)
{
    struct ifaddrs *ifaddr, *ifa;
    int family;
    char host[NI_MAXHOST];

    if (getifaddrs (&ifaddr) == -1)
        return -1;

    /* Walk through linked list, maintaining head pointer so we
       can free list later */

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        /* Get interface name and family.
           For an AF_INET* interface address, display the address */

        if (family == AF_INET )
        {
            getnameinfo (ifa->ifa_addr,
                         (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                         host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            json_t *interface = json_pack ("{s:s, s:s}",
                                               "Interface", ifa->ifa_name,
                                               "IP", host);
            json_array_append (net_state, interface);
        }
    }

    freeifaddrs (ifaddr);
    return 0;
}
