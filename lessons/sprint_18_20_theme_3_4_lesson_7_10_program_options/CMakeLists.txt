cmake_minimum_required(VERSION 3.11)

project(program_options_lesson CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(merge src/merge.cpp)

target_link_libraries(merge PRIVATE CONAN_PKG::boost Threads::Threads)
