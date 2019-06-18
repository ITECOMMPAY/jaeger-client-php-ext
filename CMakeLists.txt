cmake_minimum_required(VERSION 3.14)
project(jaeger_client)

set(CMAKE_CXX_STANDARD 11)

add_executable(jaeger_client ./src/main.cpp)