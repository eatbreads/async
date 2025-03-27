#!/bin/bash

# 服务器地址和端口
SERVER="127.0.0.1"
PORT=8082

# 总共执行的交互次数
TOTAL_INTERACTIONS=2000

# 启动的客户端连接数
CLIENTS=10  # 你可以调整这个数量来增加并发的连接数

# 每个客户端执行的交互次数
INTERACTIONS_PER_CLIENT=$((TOTAL_INTERACTIONS / CLIENTS))

# 记录总开始时间
TOTAL_START_TIME=$(date +%s%3N)

# 输出到文件
OUTPUT_FILE="interaction_durations.txt"

# 清理旧的文件
rm -f "$OUTPUT_FILE"

# 启动多个并发客户端，每个客户端执行多个交互
for i in $(seq 1 $CLIENTS); do
    {
        # 每个客户端执行多次交互
        for j in $(seq 1 $INTERACTIONS_PER_CLIENT); do
            # 记录每次交互的开始时间
          #  INTERACTION_START=$(date +%s%3N)

            # 启动一个客户端连接并发送消息
            RESPONSE=$(echo "Hello from client $i interaction $j" | nc $SERVER $PORT)

            # 打印接收到的消息
           #echo "Client $i Interaction $j received: $RESPONSE"

            # 记录交互的结束时间并计算持续时间
            INTERACTION_END=$(date +%s%3N)
            DURATION=$((INTERACTION_END - INTERACTION_START))  # 计算交互持续时间（毫秒）

            # 输出交互时长到文件
           # echo "Client $i Interaction $j duration: $DURATION ms" >> "$OUTPUT_FILE"
        done
    } &  # 并行启动每个客户端
done

# 等待所有客户端完成
wait

# 记录总共的结束时间
TOTAL_END_TIME=$(date +%s%3N)

# 计算总共的耗时
TOTAL_DURATION=$((TOTAL_END_TIME - TOTAL_START_TIME))

# 输出总耗时
echo "Total interactions duration: $TOTAL_DURATION ms"
