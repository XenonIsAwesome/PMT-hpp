cmake_minimum_required(VERSION 3.31)
project(pmthpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(pmthpp main.cpp)

target_include_directories(pmthpp PRIVATE include/)