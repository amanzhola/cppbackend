cmake_minimum_required(VERSION 3.11)

project(connection_timing CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(connection_timing main.cpp)

target_link_libraries(connection_timing CONAN_PKG::libpqxx)
