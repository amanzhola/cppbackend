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

add_executable(google_test_example
    tests/google_test_main.cpp
    src/leap_year.h
    src/leap_year.cpp
)

target_link_libraries(google_test_example PRIVATE CONAN_PKG::gtest)

add_test(NAME google_test_example COMMAND google_test_example)
