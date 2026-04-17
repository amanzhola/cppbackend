cmake_minimum_required(VERSION 3.16)
project(web_server_lesson_11 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)

find_package(Boost REQUIRED)

add_executable(web_server_lesson_11 main.cpp)
target_link_libraries(web_server_lesson_11 PRIVATE Boost::boost)
