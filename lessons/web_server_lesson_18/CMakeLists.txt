cmake_minimum_required(VERSION 3.15)
project(web_server_lesson_18 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(web_server_lesson_18
    src/main.cpp
    src/common.h
    src/http_server.h
)

target_link_libraries(web_server_lesson_18 ${CONAN_LIBS})

set_target_properties(web_server_lesson_18 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
