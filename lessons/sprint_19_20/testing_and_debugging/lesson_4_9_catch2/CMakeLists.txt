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

add_executable(catch_test_example
    tests/catch_test_main.cpp
    src/leap_year.h
    src/leap_year.cpp
)

target_link_libraries(catch_test_example PRIVATE CONAN_PKG::catch2)

add_test(NAME catch_test_example COMMAND catch_test_example)
