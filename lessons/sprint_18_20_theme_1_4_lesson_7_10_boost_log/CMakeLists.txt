cmake_minimum_required(VERSION 3.16)

project(boost_log_lesson)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(boost_log_lesson src/main.cpp)

target_link_libraries(boost_log_lesson PRIVATE CONAN_PKG::boost)
