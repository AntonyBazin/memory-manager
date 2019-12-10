cmake_minimum_required(VERSION 3.15)
project(Memory_manager)

set(CMAKE_CXX_STANDARD 14)

add_executable(Memory_manager main.cpp manager.cpp table.cpp program.cpp app.cpp myvector.cpp)