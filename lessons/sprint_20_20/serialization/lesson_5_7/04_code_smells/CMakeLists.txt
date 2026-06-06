cmake_minimum_required(VERSION 3.11)

project(code_smells_example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS serialization)

add_executable(code_smells main.cpp)

target_link_libraries(code_smells Boost::serialization)
