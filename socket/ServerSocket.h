#include <socket/BaseSocket.h>

namespace MyProject {
namespace Networking {



class ServerSocket : public BaseSocket {
public:
    explicit ServerSocket(int port, const std::string &ip = "0.0.0.0")
        : BaseSocket(AF_INET, SOCK_STREAM, 0) {
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = inet_addr(ip.c_str());

        // 绑定地址和端口
        if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }
    }

    // 开始监听
    void listenSocket(int backlog = 5) {
        if (listen(sockfd, backlog) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }
    }

    // 接受客户端连接
    int acceptClient() {
        socklen_t addrLen = sizeof(address);
        int client_fd = accept(sockfd, (struct sockaddr*)&address, &addrLen);
        if (client_fd < 0) {
            perror("Accept failed");
        }
        return client_fd;
    }
};

 }}// namespace MyProject::Network
