#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>
#include <cstring>
#include <fstream>  // 引入文件输出流
#include <unistd.h>
#include "socket/ClientSocket.h"  // 引入你的 ClientSocket 类定义

// 定义一个高并发客户端类
class HighConcurrencyClient {
public:
    HighConcurrencyClient(const std::string& server_ip, int server_port, int interactions_per_client, std::ofstream& log_file)
        : server_ip_(server_ip), server_port_(server_port), interactions_per_client_(interactions_per_client), log_file_(log_file) {}

    // 启动客户端并执行测试
    void run(int client_id) {
        auto start_time = std::chrono::high_resolution_clock::now();

        MyProject::Networking::ClientSocket client_socket;
        if (!client_socket.connectToServer(server_ip_, server_port_)) {
            log_file_ << "[Client " << client_id << "] Failed to connect to the server!" << std::endl;
            return;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> connection_duration = end_time - start_time;
        //log_file_ << "[Client " << client_id << "] Connected to the server! Connection time: " << connection_duration.count() << " seconds" << std::endl;

        // 发送指定数量的消息
        for (int i = 0; i < interactions_per_client_; ++i) {
            std::string message = "Message " + std::to_string(i + 1) + " from client";
            if (!client_socket.sendData(message)) {
                log_file_ << "[Client " << client_id << "] Error sending data to server!" << std::endl;
            } else {
                // 这里不输出消息内容，避免日志过于庞大
                std::string response = client_socket.receiveData();
            }
        }

        // 记录客户端结束时间
        auto finish_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> total_duration = finish_time - start_time;
        log_file_ << "[Client " << client_id << "] Finished all interactions. Total time: " << total_duration.count() << " seconds" << std::endl;

        // 关闭连接
        client_socket.closeSocket();
    }

private:
    std::string server_ip_;
    int server_port_;
    int interactions_per_client_;
    std::ofstream& log_file_;  // 传入日志文件对象
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "请使用: " << argv[0] << " <客户端数量> <每个的请求数>" << std::endl;
        return 1;
    }
    int clients = std::stoi(argv[1]);  // 客户端数量
    int interactions_per_client = std::stoi(argv[2]);  // 每个客户端的交互次数
    // 服务器信息
    std::string server_ip = "127.0.0.1";
    int server_port = 8082;




    // 创建日志文件
    std::ofstream log_file;
    log_file.open("client_test_log.txt", std::ios::out | std::ios::app);

    // 写入日志开始信息
    auto now = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(now);
    log_file <<std::endl<<std::endl<<std::endl<<std::endl<< "Test started at: " << std::ctime(&start_time) << std::endl;

    // 用于存储线程
    std::vector<std::thread> threads;

    // 启动多个客户端并发执行
    auto test_start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < clients; ++i) {
        threads.push_back(std::thread([&log_file, i]() {
            HighConcurrencyClient client("127.0.0.1", 8082, 200, log_file);  // 使用引用传递 log_file
            client.run(i + 1);  // 每个客户端有唯一 ID
        }));
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    auto test_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_test_duration = test_end_time - test_start_time;

    // 写入日志结束信息
    log_file << "Test completed. Total test duration: " << total_test_duration.count() << " seconds" << std::endl;
    log_file << "-----------------------------------------------" << std::endl;

    // 关闭日志文件
    log_file.close();

    std::cout << "Test completed. Check the log file 'client_test_log.txt' for details." << std::endl;

    return 0;
}
