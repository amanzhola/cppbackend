cmake_minimum_required(VERSION 3.11)

project(HelloConan CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(hello src/main.cpp)

target_link_libraries(hello
    PRIVATE
        CONAN_PKG::boost
)
