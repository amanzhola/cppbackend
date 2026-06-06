cmake_minimum_required(VERSION 3.11)

project(text_archives_example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS serialization)

add_executable(text_archives main.cpp)

target_link_libraries(text_archives Boost::serialization)
