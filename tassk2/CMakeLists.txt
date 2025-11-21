cmake_minimum_required(VERSION 3.10)
project(task2 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(task2_string lab2_string.c vector.c vector_config.c)