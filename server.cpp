// server.cpp

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib> // 用于调用 system() 函数



int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    const std::string WEB_ROOT = "./webroot";

    // 创建 socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET; //ipv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // 绑定 socket 到指定地址和端口
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // 监听连接
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    // 接受连接并处理请求
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("Accept failed");
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    // 关闭连接
    close(client_fd);
    close(server_fd);

    std::string curl_command = "curl http://example.com";
    int curl_result = system(curl_command.c_str());

    if (curl_result == 0) {
        std::cout << "curl command executed successfully!" << std::endl;
    } else {
        std::cerr << "Error executing curl command!" << std::endl;
        return 1;
    }

    return 0;
}
