cmake_minimum_required(VERSION 2.8)

project(creader C)

add_definitions(-std=gnu99 -W)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(src)

file(GLOB SRC_LISTS src/*.c)

add_executable(creader ${SRC_LISTS})
