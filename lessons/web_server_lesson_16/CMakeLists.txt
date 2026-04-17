cmake_minimum_required(VERSION 3.15)
project(web_server_lesson_16 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(demo1_dispatch_basic
    src/demo1_dispatch_basic.cpp
)

add_executable(demo2_dispatch_io1_io2_strand
    src/demo2_dispatch_io1_io2_strand.cpp
)

add_executable(demo3_quiz_letters
    src/demo3_quiz_letters.cpp
)

add_executable(demo4_abc_order
    src/demo4_abc_order.cpp
)

add_executable(demo5_def_order
    src/demo5_def_order.cpp
)

add_executable(demo6_same_strand
    src/demo6_same_strand.cpp
)

add_executable(demo7_same_strand_answer
    src/demo7_same_strand_answer.cpp
)

add_executable(demo8_two_strands
    src/demo8_two_strands.cpp
)

target_link_libraries(demo1_dispatch_basic ${CONAN_LIBS})
target_link_libraries(demo2_dispatch_io1_io2_strand ${CONAN_LIBS})
target_link_libraries(demo3_quiz_letters ${CONAN_LIBS})
target_link_libraries(demo4_abc_order ${CONAN_LIBS})
target_link_libraries(demo5_def_order ${CONAN_LIBS})
target_link_libraries(demo6_same_strand ${CONAN_LIBS})
target_link_libraries(demo7_same_strand_answer ${CONAN_LIBS})
target_link_libraries(demo8_two_strands ${CONAN_LIBS})

set_target_properties(
    demo1_dispatch_basic
    demo2_dispatch_io1_io2_strand
    demo3_quiz_letters
    demo4_abc_order
    demo5_def_order
    demo6_same_strand
    demo7_same_strand_answer
    demo8_two_strands
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
