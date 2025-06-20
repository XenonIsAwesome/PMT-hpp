cmake_minimum_required(VERSION 3.31)
project(pmt_playground)

set(CMAKE_CXX_STANDARD 20)

add_executable(pmt_playground main.cpp)

target_include_directories(pmt_playground PRIVATE include/)