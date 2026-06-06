cmake_minimum_required(VERSION 3.11)

project(supported_types_example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS serialization)

add_executable(supported_types main.cpp)

target_link_libraries(supported_types Boost::serialization)
