# 第一次测试,使用while循环配合最基础的请求,单线程配合while循环

```cpp
const char* response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";
int main()
{
    ServerSocket server(PORT);
    server.listenSocket();

    while (true) {
        int client_fd = server.acceptClient();
        if (client_fd >= 0) {
            std::cout << "Client connected!" << std::endl;

            ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
            if (bytes_sent == -1) {
                std::cerr << "Error sending message to client" << std::endl;
            }

            close(client_fd);
        }
    }
    return 0;
}


```
# 测试结果,这样朴实无华的连接,处理速度能达到一秒五万次,当然这也很合理,因为只是建立起连接之后马上断开,并没有后续的通信,所以这个朴素的while可以最快的从半连接队列中拿出东西,但是如果加上随机对话或者echo可能就不一样了
```bash
root@localhost:/project/async/test# wrk -t4 -c1000 -d10s http://127.0.0.1:8082/
Running 10s test @ http://127.0.0.1:8082/
  4 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    21.24ms  133.67ms   1.87s    96.72%
    Req/Sec    14.61k     9.49k   57.07k    72.11%
  556521 requests in 10.01s, 41.40MB read
  Socket errors: connect 0, read 84900, write 471621, timeout 90
Requests/sec:  55570.72
Transfer/sec:      4.13MB
```