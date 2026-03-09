#pragma once

#include <deque>
#include <vector>
#include <mutex>

// 环形缓冲区，存储最近的 N 帧数据
class RingBuffer {
private:
    // 这里使用 deque 来实现环形缓冲区，自动丢弃最老的数据
    std::deque<std::vector<char>> buffer_;
    std::mutex mtx_;
    size_t max_chunks_;

public:
    // 构造函数，指定最大缓存帧数
    explicit RingBuffer(size_t max_chunks = 100);

    // 推送新数据到缓冲区
    void push(const char* data, size_t len);

    // 获取当前缓冲区的快照，供新连接的客户端使用
    std::vector<std::vector<char>> snapshot();
};