cmake_minimum_required(VERSION 3.11)

project(web_server_lesson_18_static_files CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(static_server
    src/main.cpp
)

target_include_directories(static_server PRIVATE
    /home/ubuntu/.conan/data/boost/1.86.0/_/_/package/4c73f888ee1301ffed212b5fd37391dd45ff1c09/include
)

target_link_libraries(static_server
    pthread
)
