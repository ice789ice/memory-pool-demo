cmake_minimum_required(VERSION 3.15)
project(MemoryPoolDemo)

set(CMAKE_CXX_STANDARD 11)

# 头文件路径
include_directories(include)

# 可执行文件与源文件位置
add_executable(memory_pool_demo
    test/main.cpp
    src/memory_pool.cpp
)

