# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(SocketExample)

set(CMAKE_CXX_STANDARD 11)

# 编译服务器端程序
add_executable(server server.cpp)

# 编译客户端程序
add_executable(client client.cpp)
