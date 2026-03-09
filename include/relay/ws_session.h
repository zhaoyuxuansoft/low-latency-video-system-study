#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <vector>
#include <mutex>
#include <memory>
#include <string>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = asio::ip::tcp;

// WsSession代表一个 WebSocket 会话，负责与单个客户端通信
class WsSession : public std::enable_shared_from_this<WsSession> {
private:

    /* 核心优化 1：使用 Beast 的 WebSocket 流，直接在 TCP 层上操作，方便控制binary frame、socket option和连接管理，有利于构建低开销relay
    * (1) 协议握手与升级(handshake):自动处理从HTTP到WebSocket的协议转换
    * (2) 消息帧管理（Framing）：确保每个WebSocket消息都是一个独立的帧，避免了TCP的数据边界问题
    * (3) 控制帧处理（Control Frames）：自动处理ping/pong心跳和连接关闭，提升连接稳定性
    * (4) 高效的异步I/O：Beast底层使用Asio的异步机制，极大提升了并发性能和响应速度
    */
    websocket::stream<tcp::socket> ws_;

    std::mutex write_mtx_;

    /* 核心优化 5（新增）：GOP 边界对齐与防花屏机制
    * 标志位：是否正在等待下一个关键帧 (I-Frame)
    */
    bool waiting_for_iframe_ = false;

    // 辅助函数：在二进制流中扫描 MPEG-1 视频序列头特征码 (0x00 0x00 0x01 0xB3)
    size_t find_iframe_header(const std::vector<char>& data);

public:

    explicit WsSession(tcp::socket socket);

    void start();

    bool is_open() const;

    void send_binary(const std::vector<char>& data);
};