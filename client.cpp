// client.cpp

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>

int main(int argc, char *argv[]) {

    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " -h <主机IP> -p <端口号> -m <消息>\n";
        return -1;
    }

    std::string host;
    int port;
    std::string message;

    int sock = 0;
    struct sockaddr_in serv_addr;

    char buffer[1024] = {0};


    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]) == "-h") {
            host = argv[i + 1];
        } else if (std::string(argv[i]) == "-p") {
            port = std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-m") {
            message = argv[i + 1];
        }
    }

    // 创建 socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        close(sock);
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // 将 IPv4 地址从点分十进制转换为二进制格式
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(sock);
        return -1;
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        close(sock);
        return -1;
    }

    // 发送数据到服务器
    send(sock, message.c_str(), message.size(), 0); // 发送命令行参数中的消息
    std::cout << "Message sent to server: " << message << std::endl;

    // 从服务器接收数据
    read(sock, buffer, 1024);
    std::cout << "Server response: " << buffer << std::endl;

    // 关闭连接
    close(sock);

    return 0;
}

