cmake_minimum_required(VERSION 3.14)
project(jaeger_client)

set(CMAKE_CXX_STANDARD 11)

include_directories("./src")
#link_directories("/usr/local/lib/")
add_library(libphpcpp SHARED IMPORTED)
#target_link_libraries(libphpcpp)

add_executable(jaeger_client
        ./src/main.cpp
        ./src/FileReporter.cpp
                )