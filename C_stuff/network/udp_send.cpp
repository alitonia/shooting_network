#ifndef NETWORK_CLIENT_UDP_SEND_CPP
#define NETWORK_CLIENT_UDP_SEND_CPP

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <iostream>
#include <unistd.h>
#include "udp_send.hpp"
#include <arpa/inet.h>

namespace udp_send {

    int sockfd;

    void signal_handle(int sign) {
        if (sockfd) {
            close(sockfd);
        }
        std::cout << "Closing udp_recv\n";
        exit(sign);
    }


    void start(SharedQueue<pending_message *> *q) {
        struct sockaddr_in cliaddr{};

        // Creating socket file descriptor
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }
        signal(SIGINT, signal_handle);

        int len;

        struct timespec sleepTime;
        struct timespec returnTime;
        sleepTime.tv_sec = 0;
        sleepTime.tv_nsec = 10000;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
        while (true) {
            if (q->empty()) {
                nanosleep(&sleepTime, &returnTime);
            } else {
                pending_message *m;
                while (!q->empty()) {
                    m = q->pop();
//                    memset(&cliaddr, 0, sizeof(cliaddr));
//                    cliaddr.sin_family = AF_INET;
//                    cliaddr.sin_addr = m->des;
//                    cliaddr.sin_port = htons(m->port);
//
//                    len = sizeof(cliaddr);
//
//                    sendto(sockfd, (const char *) m->message, strlen(m->message),
//                           MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
//                           len);

                    free(m->message);
                    free(m);
                }
            }
        }
#pragma clang diagnostic pop
    }
}


#endif //NETWORK_CLIENT_UDP_SEND_CPP
