#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <socket/ServerSocket.h>


int main() {
    // 创建 ServerSocket 实例，绑定端口 12345
    MyProject::Networking::ServerSocket server(8082);

    // 开始监听客户端连接
    server.listenSocket(5);  // 最大客户端排队数为 5
    std::cout << "Server listening on port 8082..." << std::endl;

    while (true) {
        // 接受客户端连接
        int client_fd = server.acceptClient();
        if (client_fd >= 0) {
            std::cout << "Client connected!" << std::endl;

            char buffer[1024];
            ssize_t bytesReceived;

            // 循环接收客户端消息并回显
            while ((bytesReceived = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
                // 回显接收到的消息
                ssize_t bytesSent = send(client_fd, buffer, bytesReceived, 0);
                if (bytesSent == -1) {
                    std::cerr << "Error sending message to client" << std::endl;
                    break;
                }
            }

            if (bytesReceived == -1) {
                std::cerr << "Error receiving message from client" << std::endl;
            }

            std::cout << "Client disconnected!" << std::endl;

            // 关闭客户端连接
            close(client_fd);
        }
    }

    // 关闭服务器套接字
    server.closeSocket();

    return 0;
}