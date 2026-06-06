cmake_minimum_required(VERSION 3.11)

project(safe_file_save_example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS serialization)

add_executable(safe_file_save main.cpp)

target_link_libraries(safe_file_save Boost::serialization)
