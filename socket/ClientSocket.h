#include <socket/BaseSocket.h>

namespace MyProject {
namespace Networking {


class ClientSocket : public BaseSocket {
public:
    explicit ClientSocket() : BaseSocket(AF_INET, SOCK_STREAM, 0) {}

    // 连接服务器
    bool connectToServer(const std::string &ip, int port) {
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0) 
        {
            perror("Invalid address");
            return false;
        }

        return connect(sockfd, (struct sockaddr*)&address, sizeof(address)) == 0;
    }
};

}}  // namespace MyProject::Network
