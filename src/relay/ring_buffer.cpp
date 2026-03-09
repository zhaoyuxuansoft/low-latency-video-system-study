#include "relay/ring_buffer.h"

// 构造函数
RingBuffer::RingBuffer(size_t max_chunks) : max_chunks_(max_chunks) {}

// === 【本次新增：特征码扫描的具体实现】 ===
size_t RingBuffer::find_iframe_header(const char* data, size_t len) {
    if (len < 4) return std::string::npos;
    for (size_t i = 0; i <= len - 4; ++i) {
        if (static_cast<unsigned char>(data[i]) == 0x00 &&
            static_cast<unsigned char>(data[i + 1]) == 0x00 &&
            static_cast<unsigned char>(data[i + 2]) == 0x01 &&
            static_cast<unsigned char>(data[i + 3]) == 0xB3) {
            return i; // 找到特征码，返回偏移量
        }
    }
    return std::string::npos;
}

// === 【核心大升级：带自动重置逻辑的 push】 ===
void RingBuffer::push(const char* data, size_t len) {
    if (len == 0) return;
    std::lock_guard<std::mutex> lock(mtx_);

    // 1. 扫描新进来的这个 Chunk 里有没有 I 帧
    size_t offset = find_iframe_header(data, len);

    if (offset != std::string::npos) {
        // 🚨 发现全新 GOP！执行方案B核心逻辑：斩断过去
        buffer_.clear();

        // 正确做法：不要切片！直接把包含 I 帧的整个数据块完整存进去
        // 这样可以最大程度保全 MPEG-TS 的 188 字节对齐机制和前置的 PAT/PMT 表
        buffer_.emplace_back(data, data + len);
    }
    else {
        // 2. 没有 I 帧，说明这是当前 GOP 的后续 P 帧，正常追加
        if (buffer_.size() >= max_chunks_) {
            // 极端情况的兜底保护（比如推流端坏了，一直不发 I 帧）
            buffer_.pop_front();
        }
        buffer_.emplace_back(data, data + len);
    }
}

// 获取快照发给新网页
std::vector<std::vector<char>> RingBuffer::snapshot() {
    std::lock_guard<std::mutex> lock(mtx_);
    return std::vector<std::vector<char>>(buffer_.begin(), buffer_.end());
}