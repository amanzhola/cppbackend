cmake_minimum_required(VERSION 3.15)
project(web_server_lesson_15 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(demo1_thread_pool_defer
    src/demo1_thread_pool_defer.cpp
)

add_executable(demo2_thread_pool_post
    src/demo2_thread_pool_post.cpp
)

add_executable(demo3_io_context_defer
    src/demo3_io_context_defer.cpp
)

target_link_libraries(demo1_thread_pool_defer ${CONAN_LIBS})
target_link_libraries(demo2_thread_pool_post ${CONAN_LIBS})
target_link_libraries(demo3_io_context_defer ${CONAN_LIBS})

set_target_properties(
    demo1_thread_pool_defer
    demo2_thread_pool_post
    demo3_io_context_defer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
