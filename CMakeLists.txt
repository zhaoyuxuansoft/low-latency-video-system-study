cmake_minimum_required(VERSION 3.16)

project(low_latency_video_system_study LANGUAGES CXX)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 可选：默认使用 Release 编译
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 查找 Boost
find_package(Boost REQUIRED COMPONENTS system)

# 可执行文件
add_executable(low_latency_relay
    src/main.cpp
    src/relay/ring_buffer.cpp
    src/relay/ws_session.cpp
    src/relay/relay_server.cpp
)

# 头文件路径
target_include_directories(low_latency_relay
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# 链接 Boost
target_link_libraries(low_latency_relay
    PRIVATE
        Boost::system
)

# Windows 下如果你后面遇到 socket / thread 相关问题，可保留下面设置
if(WIN32)
    target_link_libraries(low_latency_relay
        PRIVATE
            ws2_32
            mswsock
    )
endif()

# 给不同编译器加一些常用警告和编码支持
if(MSVC)
    # 核心修复：强制 MSVC 编译器使用 UTF-8 编码读取源文件和处理执行字符集
    target_compile_options(low_latency_relay PRIVATE /W4 /utf-8)
else()
    target_compile_options(low_latency_relay PRIVATE -Wall -Wextra -Wpedantic)
endif()