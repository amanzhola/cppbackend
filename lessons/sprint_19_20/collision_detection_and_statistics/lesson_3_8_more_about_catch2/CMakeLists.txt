cmake_minimum_required(VERSION 3.11)

project(more_about_catch2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(catch2_features_tests
    tests/catch2_features_tests.cpp
    src/algorithms.h
)

target_compile_definitions(catch2_features_tests PRIVATE
    CATCH_CONFIG_MAIN
)

target_include_directories(catch2_features_tests PRIVATE
    ${CONAN_INCLUDE_DIRS}
)

target_link_libraries(catch2_features_tests PRIVATE
    ${CONAN_LIBS}
)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

catch_discover_tests(catch2_features_tests)
