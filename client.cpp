// client.cpp

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    const char *hello = "Hello from client";
    char buffer[1024] = {0};

    // 创建 socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // 将 IPv4 地址从点分十进制转换为二进制格式
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    // 发送数据到服务器
    send(sock, hello, strlen(hello), 0);
    std::cout << "Hello message sent to server" << std::endl;

    // 从服务器接收数据
    read(sock, buffer, 1024);
    std::cout << "Server response: " << buffer << std::endl;

    // 关闭连接
    close(sock);

    return 0;
}

