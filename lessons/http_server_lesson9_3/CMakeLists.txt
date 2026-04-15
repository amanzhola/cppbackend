cmake_minimum_required(VERSION 3.15)
project(http_server_lesson9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(server main.cpp)

target_link_libraries(server ${CONAN_LIBS})
