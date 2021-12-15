cmake_minimum_required(VERSION 3.16)
project(HW5)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(HW5 main.cpp)
