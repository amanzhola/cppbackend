cmake_minimum_required(VERSION 3.11)

project(lesson_6_10_compiler_profiler_friends)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    lesson_6_10_compiler_profiler_friends
    main.cpp
)

target_compile_options(
    lesson_6_10_compiler_profiler_friends
    PRIVATE
    -pg
    -O0
)

target_link_options(
    lesson_6_10_compiler_profiler_friends
    PRIVATE
    -pg
)
