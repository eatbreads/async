#include <iostream>
#include <coroutine>
#include <future>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

// 异步等待 accept
struct AcceptAwaiter {
    int server_fd;
    int client_fd = -1;
    std::future<int> future_accept;

    bool await_ready() {
        client_fd = accept(server_fd, nullptr, nullptr);
        return client_fd != -1;
    }

    void await_suspend(std::coroutine_handle<> h) {
        // 在新线程异步等待 accept，等到有连接后恢复协程
        future_accept = std::async(std::launch::async, [this] {
            int fd;
            do {
                fd = accept(server_fd, nullptr, nullptr);
                std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 避免 CPU 100% 占用
            } while (fd == -1);
            return fd;
        });

        // 异步完成后恢复协程
        std::thread([h, this]() {
            client_fd = future_accept.get();
            h.resume();
        }).detach();
    }

    int await_resume() { return client_fd; }
};

// 监听并异步 accept 客户端
struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
};
        // 构建响应报文
const char* response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";

// 假设你有一个接受连接的协程 awaiter
Task acceptConnections(int server_fd) {
    while (true) {

        int client_fd = co_await AcceptAwaiter{server_fd};

        // 将响应报文发送给客户端
        ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);

        // 处理完连接后关闭
        close(client_fd);
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8082);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    std::cout << "Server listening on port 8082..." << std::endl;

    std::thread acceptThread([&] {
        acceptConnections(server_fd);
    });

    acceptThread.join();
    return 0;
}
