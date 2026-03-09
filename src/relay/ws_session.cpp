#include "relay/ws_session.h"
#include <iostream>

WsSession::WsSession(tcp::socket socket)
    : ws_(std::move(socket)) {

    /* 核心优化 2：禁用 Nagle 算法，极大降低实时流延迟
    * Nagle 算法会在 TCP 层积累小数据包，等待更多数据一起发送，加上接收端的Delayed ACK机制，可能导致每帧数据的发送延迟增加几十毫秒，严重影响实时性
    * 优点：端到端延迟（latency）降低
    * 缺点：网络头部开销增加，但是由于视频流的数据包本身足够大，这个开销可以忽略不计
    * 以太网中的MTU通常为1500字节，TCP的MSS（最大报文段长度）通常为MTU - TCP/IP的头部40字节 = 1460字节
    * 800kbps的视频流中，单帧数据通常在几十KB以上，远大于TCP的MSS（最大报文段长度），禁用Nagle后每帧数据可以立即发送，极大提升流畅度和实时性
    * 服务器系统调用增加，但由于每帧数据足够大，发送频率相对较低，这个开销可以接受
    */
    beast::get_lowest_layer(ws_).set_option(tcp::no_delay(true));
}

// 辅助函数：在二进制流中扫描 MPEG-1 视频序列头特征码
size_t WsSession::find_iframe_header(const std::vector<char>& data) {

    for (size_t i = 0; i + 3 < data.size(); ++i) {

        if (static_cast<unsigned char>(data[i]) == 0x00 &&
            static_cast<unsigned char>(data[i + 1]) == 0x00 &&
            static_cast<unsigned char>(data[i + 2]) == 0x01 &&
            static_cast<unsigned char>(data[i + 3]) == 0xB3) {

            return i;
        }
    }

    return std::string::npos;
}

void WsSession::start() {

    beast::error_code ec;

    ws_.accept(ec);

    if (ec) {
        std::cerr << "[WS] 握手失败: " << ec.message() << std::endl;
        return;
    }

    std::cout << "[WS] 客户端已连接" << std::endl;
}

bool WsSession::is_open() const {
    return ws_.is_open();
}

void WsSession::send_binary(const std::vector<char>& data) {

    if (!ws_.is_open()) return;

    /* 核心优化 4：使用非阻塞锁保护写操作，配合 I 帧识别实现“主动丢帧”
    * 避免了直接丢弃 Chunk 导致 MPEG-TS 结构破坏和前端解码花屏的致命缺陷。
    */

    std::unique_lock<std::mutex> lock(write_mtx_, std::try_to_lock);

    // 1. 如果无法获得锁，说明网络拥塞正在发生
    if (!lock.owns_lock()) {

        // 开启“等待关键帧”模式，强制丢弃当前及后续残缺片段
        waiting_for_iframe_ = true;
        return;
    }

    // 2. 如果当前处于“拥塞恢复期”，必须等到 I 帧才能恢复发送
    if (waiting_for_iframe_) {

        size_t offset = find_iframe_header(data);

        if (offset == std::string::npos) {

            // 没找到关键帧头，说明这还是 P 帧/B 帧的残余部分，继续无情丢弃
            return;
        }

        // 找到了！说明全新的 GOP（图像组）开始了
        waiting_for_iframe_ = false;

        // 裁剪掉包头前无用的旧帧残渣，精准从 I 帧特征码处开始发送
        std::vector<char> valid_data(data.begin() + offset, data.end());

        beast::error_code ec;
        ws_.binary(true);
        ws_.write(asio::buffer(valid_data), ec);

        if (ec) {
            std::cerr << "[WS] 恢复I帧后写入失败: " << ec.message() << std::endl;
        }

        return;
    }

    // 3. 正常情况：拿到锁且网络通畅，直接阻塞写入
    beast::error_code ec;

    ws_.binary(true);
    ws_.write(asio::buffer(data), ec);

    if (ec) {
        std::cerr << "[WS] 写入失败: " << ec.message() << std::endl;
    }
}