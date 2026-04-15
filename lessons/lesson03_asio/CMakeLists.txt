cmake_minimum_required(VERSION 3.11)

project(HelloAsio CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(hello_asio src/main.cpp)
target_link_libraries(hello_asio PRIVATE Threads::Threads)
