#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
#include <sstream>
#include <thread>

struct Config
{
    std::string host;
    int32_t port;
    size_t buffer_size;
    size_t thread_number;
    std::string dir;
};

int main(int argc, char *argv[])
{
    Config config;

    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] << " -h <主机IP> -p <端口号>\n";
        return 1;
    }

    for (int i = 1; i < argc; i += 2)
    {
        if (std::string(argv[i]) == "-h")
        {
            config.host = argv[i + 1];
        }
        else if (std::string(argv[i]) == "-p")
        {
            config.port = std::atoi(argv[i + 1]);
        }
    }

    config.buffer_size = 256;    // 默认缓冲区大小
    config.thread_number = 4;    // 默认线程数量
    config.dir = "/path/to/dir"; // 默认目录

    int server_fd, client_fd, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[256];

    // 创建 socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        close(server_fd);
        return 1;
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET; // ipv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config.port);

    // 绑定 socket 到指定地址和端口
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // 监听连接
    if (listen(server_fd, 5) == -1)
    {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << config.port << std::endl;

    // 阻塞 接受连接并处理请求
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1)
    {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    memset(buffer, 0, sizeof(buffer));

    // 从客户端接收数据
    n = read(client_fd, buffer, sizeof(buffer));
    if (n < 0)
    {
        perror("Read error");
        close(client_fd);
        close(server_fd);
        return 1;
    }

    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::ostringstream oss;
    // oss << "[" << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "] ";
    oss << "[" << ltm->tm_year + 1900 << "-" << ltm->tm_mon + 1 << "-" << ltm->tm_mday << " ";
    oss << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "] ";
    oss << "Thread ID: " << std::this_thread::get_id() << " ";
    oss << "Message: " << buffer;

    std::string response = oss.str();

    // 回复客户端
    // n = write(client_fd, buffer, n); // 回复收到的消息
    n = write(client_fd, response.c_str(), response.size());
    if (n == -1)
    {
        perror("Error writing to socket");
        close(client_fd);
        close(server_fd);
        return 1;
    }

    // 关闭连接
    close(client_fd);
    close(server_fd);

    return 0;
}
