cmake_minimum_required(VERSION 3.17)
project(network_client)

set(CMAKE_CXX_STANDARD 20)

add_executable(network_client tcp_client.cpp tcp_server.cpp udp_client.cpp udp_server.cpp)