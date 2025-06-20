cmake_minimum_required(VERSION 3.31)
project(pmthpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(pmthpp main.cpp)

add_executable(pmt-example example.cpp)

target_include_directories(pmt-example PRIVATE include/)
target_include_directories(pmthpp PRIVATE include/)