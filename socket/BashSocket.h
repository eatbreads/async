#ifndef BASESOCKET_H
#define BASESOCKET_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <arpa/inet.h>

namespace MyProject {
namespace Networking {

class BaseSocket {
protected:
    int sockfd;
    struct sockaddr_in address;

public:
    explicit BaseSocket(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0) {
        sockfd = socket(domain, type, protocol);
        if (sockfd < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // 发送数据
    bool sendData(const std::string &data) {
        return send(sockfd, data.c_str(), data.size(), 0) != -1;
    }

    // 接收数据
    std::string receiveData(size_t bufferSize = 1024) {
        char buffer[bufferSize];
        memset(buffer, 0, bufferSize);
        ssize_t bytesReceived = recv(sockfd, buffer, bufferSize - 1, 0);
        return (bytesReceived > 0) ? std::string(buffer) : "";
    }

    // 关闭 Socket
    virtual void closeSocket() {
        if (sockfd != -1) {
            close(sockfd);
            sockfd = -1;
        }
    }

    // 设置 Socket 选项
    /**
     * @brief 设置 Socket 选项
     *
     * 这个函数封装了 `setsockopt()`，用于配置 socket 的行为，例如：
     * - 端口复用 (`SO_REUSEADDR`)
     * - 关闭 Nagle 算法 (`TCP_NODELAY`)
     * - 启用 Keep-Alive (`SO_KEEPALIVE`)
     *
     * @param level 选项所属协议层（`SOL_SOCKET`, `IPPROTO_TCP` 等）
     * @param optname 具体的选项（如 `SO_REUSEADDR`, `TCP_NODELAY`）
     * @param value 选项值（一般是 `int`）
     */
    void setSocketOption(int level, int optname, int value) {
        setsockopt(sockfd, level, optname, &value, sizeof(value));
    }

    // 设置超时时间
    void setTimeout(int seconds) {
        struct timeval timeout{};
        timeout.tv_sec = seconds;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    }

    // 设置非阻塞模式
    void setNonBlocking(bool enable) {
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (enable)
            fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        else
            fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);
    }

    // 获取 socket 文件描述符
    int getSocketFD() const { return sockfd; }

    // 析构函数（自动关闭）
    virtual ~BaseSocket() { closeSocket(); }
};


} // namespace Networking
} // namespace MyProject
#endif // BASESOCKET_H
