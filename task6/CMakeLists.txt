cmake_minimum_required(VERSION 3.10)
project(task6)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


add_executable(${CMAKE_PROJECT_NAME} main.c foo.c)