#ifndef NETWORK_CLIENT_RESOLVE_HOSTNAME_CPP
#define NETWORK_CLIENT_RESOLVE_HOSTNAME_CPP

#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>

struct in_addr *resolve_hostname(const char *hostname) {
    auto host_info = gethostbyname(hostname);
    if (host_info == NULL) {
        fprintf(stderr, "Unknown host %s.\n", hostname);
        return nullptr;
    }
    return (struct in_addr *) host_info->h_addr;
}

#endif //NETWORK_CLIENT_RESOLVE_HOSTNAME_CPP
