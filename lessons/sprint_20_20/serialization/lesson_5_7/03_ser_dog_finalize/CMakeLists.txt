cmake_minimum_required(VERSION 3.11)

project(ser_dog_example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS serialization)

add_executable(ser_dog main.cpp)

target_link_libraries(ser_dog Boost::serialization)
