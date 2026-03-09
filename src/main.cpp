#include "relay/relay_server.h"

#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#endif

namespace asio = boost::asio;

int main() {
    // --- 新增下面这几行，强行将 Windows 控制台输出设为 UTF-8 ---
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    // --------------------------------------------------------
    try {

        asio::io_context io;

        RelayServer server(io, 9001, 9999);

        std::cout << "========================================" << std::endl;
        std::cout << "中转服务器启动成功" << std::endl;
        std::cout << "推流地址 (TCP): 127.0.0.1:9001" << std::endl;
        std::cout << "播放地址 (WS) : ws://127.0.0.1:9999" << std::endl;
        std::cout << "========================================" << std::endl;

        server.start();

        // 阻塞主线程
        for (;;) {

            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    }

    catch (const std::exception& e) {

        std::cerr << "致命错误: " << e.what() << std::endl;

        return 1;
    }

    return 0;
}