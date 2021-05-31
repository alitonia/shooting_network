#ifndef NETWORK_CLIENT_UDP_RECV_CPP
#define NETWORK_CLIENT_UDP_RECV_CPP

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <iostream>
#include "udp_recv.hpp"
#include <arpa/inet.h>
#include <nlohmann/json.hpp>
#include "dotenv.h"

using namespace dotenv;

using json = nlohmann::json;



namespace udp_recv {
    int sockfd;

    void signal_handle(int sign) {
        if (sockfd) {
            close(sockfd);
        }
        std::cout << "Closing udp_recv\n";
        exit(sign);
    }


    void start(SharedQueue<char *> *q, int _recv_port) {
        int PY_PORT=atoi(env["PY_PORT"].c_str());
        int NODE_PORT=atoi(env["NODE_PORT"].c_str());
        int recv_port = atoi(env["C_PORT"].c_str());

        printf("Config: listen %d | py: %d | node: %d\n", recv_port, PY_PORT, NODE_PORT);

        char buffer[MAXLINE];
        struct sockaddr_in servaddr{}, cliaddr{};

        // Creating socket file descriptor
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }
        signal(SIGINT, signal_handle);

        memset(&servaddr, 0, sizeof(servaddr));
        memset(&cliaddr, 0, sizeof(cliaddr));

        // Filling server information
        servaddr.sin_family = AF_INET; // IPv4
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(recv_port);

        // Bind the socket with the server address
        if (bind(sockfd, (const struct sockaddr *) &servaddr,
                 sizeof(servaddr)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        int len, n;

        len = sizeof(servaddr); //len is value

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

        int length;
        while (true) {
            n = recvfrom(sockfd, (char *) buffer, MAXLINE,
                         MSG_WAITALL, (struct sockaddr *) &servaddr,
                         reinterpret_cast<socklen_t *>(&len));
            buffer[n] = '\0';
            length = strlen(buffer);

            int dest_port = (int) ntohs(servaddr.sin_port);
            int is_py_package = 0;

            if (length > 0) {
            //forward
                if (length >=4 &&buffer[length-4]=='-'&&buffer[length-3]=='-'&& buffer[length-2]=='p' && buffer[length-1] == 'y'){
                    is_py_package = 1;
                    buffer[strlen(buffer)-4] = '\0';
                }
                printf("  //_s: %d\n", is_py_package);
//                char *cm = (char *) malloc(sizeof(char) * strlen(buffer));
//                strcpy(cm, buffer);

                memset(&cliaddr, 0, sizeof(cliaddr));
                cliaddr.sin_family = AF_INET;
                cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

                if(is_py_package == 1){
                    cliaddr.sin_port = htons(NODE_PORT);
                }else{
                    cliaddr.sin_port = htons(PY_PORT);
                }
//
//                if(is_py_package!=1){
//                try{
//                auto structured_msg = json::parse(buffer);
//                if(structured_msg["type"] == 4){
//                    printf("SOmething good here\n");
//                    auto s = (structured_msg["player_list"].dump()).c_str();
//                    std::cout << structured_msg["player_list"].dump()<<'\n';
//                    std::cout << s<<'\n';
//
//                    json j;
//                    j["type"] = 4;
//                    j["x"] = s;
//                    sendto(sockfd, (const char *) s, strlen(s),
//                                           MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
//                                           sizeof(cliaddr));
//                                           continue;
//                                           }
//                }
//}

                sendto(sockfd, (const char *) buffer, strlen(buffer),
                       MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
                       sizeof(cliaddr));
                std::cout<< buffer;
            }
        }
#pragma clang diagnostic pop
    }
}


#endif //NETWORK_CLIENT_UDP_RECV_CPP
