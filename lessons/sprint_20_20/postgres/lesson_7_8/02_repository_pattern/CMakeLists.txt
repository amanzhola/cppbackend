cmake_minimum_required(VERSION 3.11)

project(repository_pattern_example CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(repository_example main.cpp)

target_link_libraries(repository_example
    CONAN_PKG::libpqxx
    CONAN_PKG::boost
)
