cmake_minimum_required(VERSION 3.11)

project(configuring_catch2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(math_tools_lib STATIC
    src/math_tools.h
    src/math_tools.cpp
)

add_executable(math_app
    src/main.cpp
)

target_link_libraries(math_app PRIVATE
    math_tools_lib
)

add_executable(math_tests
    tests/math_tools_tests.cpp
)

target_compile_definitions(math_tests PRIVATE
    CATCH_CONFIG_MAIN
)

target_include_directories(math_tests PRIVATE
    ${CONAN_INCLUDE_DIRS}
)

target_link_libraries(math_tests PRIVATE
    math_tools_lib
    ${CONAN_LIBS}
)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

catch_discover_tests(math_tests)
