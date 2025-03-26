#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <socket/ServerSocket.h>

#define PORT 8082
using namespace MyProject::Networking;

int main()
{
    int server_fd,new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};


    //创建服务器
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0)
    {
        perror("socket faild");
        exit(EXIT_FAILURE);
    }
    
    // 绑定地址和端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
       
       
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    // 监听端口，最多 3 个客户端排队
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port " << PORT << std::endl;
        // 接受客户端连接
    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, 1024);
    std::cout << "Received from client: " << buffer << std::endl;

    // 发送响应
    const char* response = "Hello from server";
    send(new_socket, response, strlen(response), 0);
    
    
    // 关闭连接
    close(new_socket);
    close(server_fd);
    return 0;
}