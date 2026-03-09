#pragma once

#include "relay/ring_buffer.h"
#include "relay/ws_session.h"

#include <boost/asio.hpp>

#include <vector>
#include <memory>
#include <mutex>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class RelayServer {

private:

    asio::io_context& io_;

    tcp::acceptor tcp_acceptor_;
    tcp::acceptor ws_acceptor_;

    RingBuffer ring_;

    std::mutex clients_mtx_;

    std::vector<std::shared_ptr<WsSession>> clients_;

    void accept_tcp_loop();

    void accept_ws_loop();

    void broadcast(const char* data, size_t len);

public:

    RelayServer(asio::io_context& io, unsigned short tcp_port, unsigned short ws_port);

    void start();
};