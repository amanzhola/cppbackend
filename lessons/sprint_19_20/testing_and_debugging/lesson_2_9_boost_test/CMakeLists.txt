enable_testing()

cmake_minimum_required(VERSION 3.11)

project(samples CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(application
    src/main.cpp
    src/leap_year.h
    src/leap_year.cpp
)

add_executable(boost_test_example
    tests/boost_test_main.cpp
    src/leap_year.h
    src/leap_year.cpp
)

add_test(NAME boost_test_example COMMAND boost_test_example)
