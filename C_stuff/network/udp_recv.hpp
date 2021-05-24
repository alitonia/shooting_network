#ifndef NETWORK_CLIENT_UDP_RECV_HPP
#define NETWORK_CLIENT_UDP_RECV_HPP

#include "../thread_safe_ds/shared_queue.cpp"

namespace udp_recv {

#define PORT     8996
#define MAXLINE 1024

    void signal_handle(int sign);

    void start(SharedQueue<char *> *q, int recv_port = PORT);
}


#endif //NETWORK_CLIENT_UDP_RECV_HPP
