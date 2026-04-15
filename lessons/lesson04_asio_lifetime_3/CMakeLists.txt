cmake_minimum_required(VERSION 3.15)
project(lesson04_asio_lifetime_3 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(lesson04_asio_lifetime_3 src/main.cpp)
target_link_libraries(lesson04_asio_lifetime_3 ${CONAN_LIBS})
