cmake_minimum_required(VERSION 3.16)

project(logging_patterns_part5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(logging_part5 src/main.cpp)

target_link_libraries(logging_part5 pthread)
