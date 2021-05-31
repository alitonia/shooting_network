#include "encode_decode/encode_decode.hpp"
#include <iostream>
#include "network/udp_recv.hpp"
#include "network/udp_send.hpp"

#include "thread_safe_ds/shared_queue.cpp"
#include <thread>
#include "dotenv.h"

using namespace dotenv;

int main() {
    env.load_dotenv("../.env");
    auto x = new SharedQueue<char *>;
    udp_recv::start(x);
    return 0;
}