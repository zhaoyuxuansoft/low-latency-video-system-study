#include "relay/ring_buffer.h"

// 构造函数
RingBuffer::RingBuffer(size_t max_chunks)
    : max_chunks_(max_chunks) {
}

// 推送新数据到缓冲区
void RingBuffer::push(const char* data, size_t len) {

    if (len == 0) return;

    std::lock_guard<std::mutex> lock(mtx_);

    // 如果缓冲区满了，丢弃最老的一帧
    // 这样可以保证新连接的客户端从最新的位置开始观看
    if (buffer_.size() >= max_chunks_) {
        buffer_.pop_front();
    }

    // 将新数据复制到缓冲区
    buffer_.emplace_back(data, data + len);
}

// 获取当前缓冲区的快照，供新连接的客户端使用
std::vector<std::vector<char>> RingBuffer::snapshot() {

    std::lock_guard<std::mutex> lock(mtx_);

    return std::vector<std::vector<char>>(buffer_.begin(), buffer_.end());
}