cmake_minimum_required(VERSION 3.11)

# Проект называется TCP_UDP_Practice и написан на C++
project(TCP_UDP_Practice CXX)

# Стандарт C++20
set(CMAKE_CXX_STANDARD 20)

# Подключаем Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# 4 программы
add_executable(TCPServer tcp_server.cpp)
add_executable(TCPClient tcp_client.cpp)
add_executable(UDPServer udp_server.cpp)
add_executable(UDPClient udp_client.cpp)

# Потоки
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(TCPServer PRIVATE Threads::Threads)
target_link_libraries(TCPClient PRIVATE Threads::Threads)
target_link_libraries(UDPServer PRIVATE Threads::Threads)
target_link_libraries(UDPClient PRIVATE Threads::Threads)
