#include "relay/relay_server.h"

#include <boost/beast/core.hpp>

#include <iostream>
#include <thread>
#include <vector>

namespace beast = boost::beast;

RelayServer::RelayServer(asio::io_context& io, unsigned short tcp_port, unsigned short ws_port)

    : io_(io),
    tcp_acceptor_(io, tcp::endpoint(tcp::v4(), tcp_port)),
    ws_acceptor_(io, tcp::endpoint(tcp::v4(), ws_port)),
    ring_(150) { // 稍微增加一点缓存长度，容纳约 1-2 秒的数据
}

void RelayServer::start() {

    // 启动两个监听线程
    std::thread([this]() { accept_tcp_loop(); }).detach();
    std::thread([this]() { accept_ws_loop(); }).detach();
}

void RelayServer::accept_tcp_loop() {

    for (;;) {

        try {

            tcp::socket socket(io_);

            tcp_acceptor_.accept(socket);

            // 核心优化 3：推流端也禁用 Nagle
            socket.set_option(tcp::no_delay(true));

            std::cout << "[TCP] FFmpeg 推流端已连接" << std::endl;

            std::thread([this, s = std::move(socket)]() mutable {

                std::vector<char> buf(64 * 1024);

                for (;;) {

                    beast::error_code ec;

                    size_t n = s.read_some(asio::buffer(buf), ec);

                    if (ec) {

                        std::cerr << "[TCP] 推流中断: " << ec.message() << std::endl;

                        break;
                    }

                    if (n > 0) {

                        ring_.push(buf.data(), n);

                        broadcast(buf.data(), n);
                    }
                }

                }).detach();

        }

        catch (const std::exception& e) {

            std::cerr << "[TCP] 接收异常: " << e.what() << std::endl;
        }
    }
}

void RelayServer::accept_ws_loop() {

    for (;;) {

        try {

            tcp::socket socket(io_);

            ws_acceptor_.accept(socket);

            auto client = std::make_shared<WsSession>(std::move(socket));

            client->start();

            {

                std::lock_guard<std::mutex> lock(clients_mtx_);

                clients_.push_back(client);
            }

            // 新连接：发送历史数据，实现“秒开”效果
            auto history = ring_.snapshot();

            for (auto& chunk : history) {

                client->send_binary(chunk);
            }
        }

        catch (const std::exception& e) {

            std::cerr << "[WS] 接收异常: " << e.what() << std::endl;
        }
    }
}

void RelayServer::broadcast(const char* data, size_t len) {

    std::vector<std::shared_ptr<WsSession>> alive;

    {

        std::lock_guard<std::mutex> lock(clients_mtx_);

        for (auto& c : clients_) {

            if (c && c->is_open()) {

                alive.push_back(c);
            }
        }

        clients_ = alive; // 清理掉已经断开的客户端
    }

    std::vector<char> packet(data, data + len);

    // 核心优化 4：并行广播，但在发送时检查拥塞
    for (auto& c : alive) {

        // 这里可以在独立线程中发送，但在小规模实验中直接调用即可
        c->send_binary(packet);
    }
}