cmake_minimum_required(VERSION 3.10)
project(udp C)

set(CMAKE_C_STANDARD 11)

add_executable(udpclient udp_client.c)
add_executable(udpserver udp_server.c)
