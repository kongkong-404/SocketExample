#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
#include <thread>
#include <sstream>

struct Config
{
    std::string host;
    int32_t port;
    size_t buffer_size;
    size_t thread_number;
    std::string dir;
};

void handleClient(int client_fd, const Config &config)
{
    // char buffer[256];
    std::vector<char> buffer(config.buffer_size);
    int n;
    std::memset(buffer.data(), 0, sizeof(buffer));

    // 从客户端接收数据
    n = read(client_fd, buffer.data(), sizeof(buffer));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket\n";
        close(client_fd);
        return;
    }

    // 获取当前时间和线程 ID
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::ostringstream oss;
    oss << "[" << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday << " ";
    oss << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "] ";
    oss << "Thread ID: " << std::this_thread::get_id() << " ";
    oss << "Message: " << buffer.data();

    // 处理客户端消息
    std::cout << "Received message from client: " << buffer.data() << std::endl;

    // 回复客户端
    std::string response = oss.str();
    n = write(client_fd, response.c_str(), response.size());
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket\n";
        close(client_fd);
        return;
    }

    close(client_fd);
}

int main(int argc, char *argv[])
{

    Config config;

    config.host = "127.0.0.1";
    config.port = 8080;
    config.buffer_size = 256;
    config.thread_number = 4;
    config.dir = "/path/to/dir";

    if (argc != 5 && argc != 1)
    {
        std::cerr << "Usage: " << argv[0] << " -h <主机IP> -p <端口号> or" << argv[0] << "./server use default configuration 127.0.0.1:8080\n";
        return 1;
    }

    // 解析命令行参数
    for (int i = 1; i < argc; i += 2)
    {
        if (i + 1 < argc)
        {
            if (std::string(argv[i]) == "-h")
                config.host = argv[i + 1];
            else if (std::string(argv[i]) == "-p")
                config.port = std::atoi(argv[i + 1]);
        }
    }

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pid_t pid; //

    // 创建 socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config.port);

    // 绑定 socket 到指定地址和端口
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // 监听连接
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    while (true)
    {
        // 接受连接
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        // 创建子进程
        pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            close(client_fd);
            continue;
        }
        else if (pid == 0)
        {                                    // 子进程
            close(server_fd);                // 关闭子进程中不需要的文件描述符
            handleClient(client_fd, config); // 处理客户端请求
            exit(0);                         // 处理完毕后退出子进程
        }
        else
        {                     // 父进程
            close(client_fd); // 关闭父进程中不需要的文件描述符
        }
    }

    close(server_fd); // 关闭服务器 socket
    return 0;
}
