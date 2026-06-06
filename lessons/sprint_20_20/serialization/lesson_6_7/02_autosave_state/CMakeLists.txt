cmake_minimum_required(VERSION 3.11)

project(autosave_state_example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS serialization)

add_executable(autosave_state main.cpp)

target_link_libraries(autosave_state Boost::serialization)
