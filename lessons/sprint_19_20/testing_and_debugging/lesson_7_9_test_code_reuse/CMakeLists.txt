enable_testing()

cmake_minimum_required(VERSION 3.11)

project(test_code_reuse_example CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(tv_tests
    tests/tv_tests.cpp
    src/tv.h
    src/tv.cpp
)

add_test(NAME tv_tests COMMAND tv_tests)
