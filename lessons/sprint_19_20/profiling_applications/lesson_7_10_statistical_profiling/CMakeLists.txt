cmake_minimum_required(VERSION 3.11)

project(lesson_7_10_statistical_profiling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    lesson_7_10_statistical_profiling
    main.cpp
)

target_compile_options(
    lesson_7_10_statistical_profiling
    PRIVATE
    -O2
    -g
    -fno-omit-frame-pointer
)
