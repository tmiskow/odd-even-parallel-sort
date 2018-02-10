cmake_minimum_required(VERSION 3.9)
project(sort C)

set(CMAKE_C_STANDARD 11)

add_executable(sort main.c shared_memory.h shared_memory.c error.c error.h)
target_link_libraries(sort pthread)