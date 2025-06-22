cmake_minimum_required(VERSION 3.18)
project(PMT)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME}_LIB INTERFACE)
target_include_directories(${PROJECT_NAME}_LIB INTERFACE include/)

# Example file
add_executable(${PROJECT_NAME}-example example.cpp)
target_link_libraries(${PROJECT_NAME}-example ${PROJECT_NAME}_LIB)

# Playground for testing
add_executable(${PROJECT_NAME}-playground main.cpp)
target_link_libraries(${PROJECT_NAME}-playground ${PROJECT_NAME}_LIB)
