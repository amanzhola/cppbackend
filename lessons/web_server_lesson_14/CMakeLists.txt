cmake_minimum_required(VERSION 3.15)
project(web_server_lesson_14 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(demo1_post_single_thread
    src/demo1_post_single_thread.cpp
)

add_executable(demo2_post_other_thread
    src/demo2_post_other_thread.cpp
)

add_executable(demo3_post_multi_thread
    src/demo3_post_multi_thread.cpp
)

add_executable(demo4_strand_order
    src/demo4_strand_order.cpp
)

add_executable(demo5_output_order
    src/demo5_output_order.cpp
)

target_link_libraries(demo1_post_single_thread ${CONAN_LIBS})
target_link_libraries(demo2_post_other_thread ${CONAN_LIBS})
target_link_libraries(demo3_post_multi_thread ${CONAN_LIBS})
target_link_libraries(demo4_strand_order ${CONAN_LIBS})
target_link_libraries(demo5_output_order ${CONAN_LIBS})

set_target_properties(
    demo1_post_single_thread
    demo2_post_other_thread
    demo3_post_multi_thread
    demo4_strand_order
    demo5_output_order
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
