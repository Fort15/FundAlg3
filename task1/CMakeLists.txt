cmake_minimum_required(VERSION 3.10)
project(task1)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

add_executable(${CMAKE_PROJECT_NAME} main.c foo.c)

target_link_libraries(task1 m)