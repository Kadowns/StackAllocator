cmake_minimum_required(VERSION 3.17)
project(stack_allocator)

set(CMAKE_CXX_STANDARD 20)

add_executable(stack_allocator example/main.cpp)

target_include_directories(stack_allocator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})