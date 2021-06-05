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
#include <list>
#include <iterator>
#include <string.h>

#include <chrono>
#include <ctime> 

using namespace dotenv;

using json = nlohmann::json;


namespace udp_recv {

    struct endpoint{
        char ip[20];
        int port;
    };

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
        const char* NODEIP = env["NODEIP"].c_str();
        
        int is_P2P = atoi(env["P2P"].c_str());

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
        std::list<endpoint>  ep_list;

        auto last_send = std::chrono::system_clock::now();
        
        // timeout for sending keep alive msg
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            perror("Error");
        }

        while (true) {
            memset(buffer, 0, sizeof(char)*MAXLINE);

            n = recvfrom(sockfd, (char *) buffer, MAXLINE,
               MSG_WAITALL, (struct sockaddr *) &servaddr,
               reinterpret_cast<socklen_t *>(&len));
            

            buffer[n] = '\0';

            // artificially create packages
            if (strlen(buffer) == 0)
            {
                // can be timout or 0 size package
                auto current_time = std::chrono::system_clock::now();
                auto time_elapsed = current_time - last_send;

                if (time_elapsed.count() >= 1)
                {
                    //send keep alive
                    if (is_P2P)
                    {
                        strcpy(buffer, "friendly_keep_alive--cp");
                    }else{
                        strcpy(buffer, "friendly_keep_alive--nd");
                    }
                    printf("Hello %s\n", buffer);
                    last_send = current_time;
                }
                
            }


            length = strlen(buffer);

            int dest_port = (int) ntohs(servaddr.sin_port);

            int is_to_node = 0; //nd
            int is_to_C = 0; //cp
            int is_self_config = 0; //cf
            int is_to_py = 0;

            if (length > 0) {
            //forward
                if (length >=4 &&buffer[length-4]=='-'&&buffer[length-3]=='-'&& buffer[length-2]=='n' && buffer[length-1] == 'd'){
                    is_to_node = 1;
                    buffer[strlen(buffer)-4] = '\0';
                }else if (length >=4 &&buffer[length-4]=='-'&&buffer[length-3]=='-'&& buffer[length-2]=='c' && buffer[length-1] == 'p'){
                    is_to_C = 1;
                    buffer[strlen(buffer)-4] = '\0';
                }else if (length >=4 &&buffer[length-4]=='-'&&buffer[length-3]=='-'&& buffer[length-2]=='c' && buffer[length-1] == 'f'){
                   is_self_config = 1;
                   buffer[strlen(buffer)-4] = '\0';
               }else{
                is_to_py = 1;
               }

               printf("\nb: %s\n", buffer);
               printf("\n Node->%d C->%d Self->%d\n", is_to_node, is_to_C, is_self_config);
//                char *cm = (char *) malloc(sizeof(char) * strlen(buffer));
//                strcpy(cm, buffer);

               memset(&cliaddr, 0, sizeof(cliaddr));
               cliaddr.sin_family = AF_INET;
               cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

               if(is_to_node == 1){
                cliaddr.sin_port = htons(NODE_PORT);
                }else{
                    cliaddr.sin_port = htons(PY_PORT);
                }
//

                //special parse for join_room to save address
            if(is_to_py==1){
                    // find first space
                int i =-1;
                for(i=0; i< strlen(buffer); i++){
                    if(buffer[i] == ' '){
                        break;
                    }
                }
                std::cout << i <<'\n';
                char *ptr = nullptr;
                if(i!=-1 && (i < strlen(buffer)-1)){
                    ptr = buffer+i+1;
                    printf("Parsed: %s\n", ptr);
                }
                if(ptr && strlen(ptr) >0 && ptr[0] == '{'){
                    try{
                        auto structured_msg = json::parse(ptr);
                        if(structured_msg["type"] == 4){
                            ep_list.clear();
                            printf("Something good here\n");
                            auto s = (structured_msg["player_list"].dump()).c_str();
//                        std::cout << structured_msg["player_list"].dump()<<'\n';
//                        std::cout << s<<'\n';
                            auto iterable_l = structured_msg["IPs"];

                            std::cout << iterable_l <<'\n';

                            for (json::iterator it = iterable_l.begin(); it != iterable_l.end(); ++it) {
                                printf("-------\n");
                                auto p = it.value().get<std::string>();

                                std::cout << p  << "\n"<<std::endl;
                                const char* v = p.c_str();
                                printf("j: %s %ld, ord %ld\n", v, strlen(v), p.length());


                                int j = -1;
                                for(j=0; j< strlen(v);j++){
                                    if(v[j] ==':'){
                                        break;
                                    }
                                }
                                endpoint x;

                                if(j!=-1 && j < strlen(v)-2){
                                    strncpy( x.ip, v, j*sizeof(char) );
                                    x.port = atoi(v+j+1);
                                    ep_list.push_back(x);
                                }

                                printf("%s, %d -> %s %d\n",v, j, x.ip, x.port);
                            }
                        }
                    } catch(json::basic_json::parse_error x){
                       throw(x);
//                     std::cout<<"this errors parser\n";
                   }
               }
           }
//}
       if(is_self_config ==0 && is_to_C == 0){
            last_send = std::chrono::system_clock::now();
            
            sendto(sockfd, (const char *) buffer, strlen(buffer),
             MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
             sizeof(cliaddr));
        }

        std::cout<< buffer;

       if(is_to_C == 1){
            last_send = std::chrono::system_clock::now();
            // printf("Send spam\n");
                // spam send
           for(auto f= ep_list.begin(); f!= ep_list.end(); f++){

            memset(&cliaddr, 0, sizeof(cliaddr));
            cliaddr.sin_family = AF_INET;
            cliaddr.sin_addr.s_addr = inet_addr(f->ip);
            cliaddr.sin_port = htons(f->port);

            sendto(sockfd, (const char *) buffer, strlen(buffer),
             MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
             sizeof(cliaddr));

            printf("_\n");
            printf("To: %s %d %s\n", f->ip, f->port, buffer);
            printf("_\n");


        }
    }
//    for(auto f= ep_list.begin(); f!= ep_list.end(); f++){
//        printf("_\n");
//        printf("Out: %s %d\n", f->ip, f->port);
//        printf("_\n");
//
//    }

}
}
#pragma clang diagnostic pop
}
}


#endif //NETWORK_CLIENT_UDP_RECV_CPP
