cmake_minimum_required(VERSION 3.16)
project(web_server_lesson_13 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_executable(demo1_parallel_timers src/demo1_parallel_timers.cpp)
target_link_libraries(demo1_parallel_timers PRIVATE Boost::boost Threads::Threads)

add_executable(demo2_strand_timers src/demo2_strand_timers.cpp)
target_link_libraries(demo2_strand_timers PRIVATE Boost::boost Threads::Threads)

add_executable(demo3_order_race src/demo3_order_race.cpp)
target_link_libraries(demo3_order_race PRIVATE Boost::boost Threads::Threads)

add_executable(demo4_order_strand src/demo4_order_strand.cpp)
target_link_libraries(demo4_order_strand PRIVATE Boost::boost Threads::Threads)
