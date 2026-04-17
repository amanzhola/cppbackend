cmake_minimum_required(VERSION 3.15)
project(web_server_lesson_17 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(demo1_signal_set_shutdown
    src/demo1_signal_set_shutdown.cpp
    src/common.h
)

add_executable(demo2_server_architecture_stub
    src/demo2_server_architecture_stub.cpp
    src/common.h
)

add_executable(demo3_async_accept_stub
    src/demo3_async_accept_stub.cpp
    src/common.h
)

target_link_libraries(demo1_signal_set_shutdown ${CONAN_LIBS})
target_link_libraries(demo2_server_architecture_stub ${CONAN_LIBS})
target_link_libraries(demo3_async_accept_stub ${CONAN_LIBS})

set_target_properties(
    demo1_signal_set_shutdown
    demo2_server_architecture_stub
    demo3_async_accept_stub
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
