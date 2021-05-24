#ifndef NETWORK_CLIENT_UDP_SEND_HPP
#define NETWORK_CLIENT_UDP_SEND_HPP

#include "../thread_safe_ds/shared_queue.cpp"

namespace udp_send {

#define MAXLINE 1024
    struct pending_message {
        char *message;
//        struct in_addr des;
        uint16_t port;
    };

    void signal_handle(int sign);

    void start(SharedQueue<pending_message *> *q);
}


#endif //NETWORK_CLIENT_UDP_SEND_HPP
