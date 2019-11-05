cmake_minimum_required(VERSION 3.16)
project(algorithm_1 C)

set(CMAKE_C_STANDARD 99)

add_executable(algorithm_1 main.c)

find_package(Threads REQUIRED)
target_link_libraries(algorithm_1 Threads::Threads)