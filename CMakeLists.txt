cmake_minimum_required(VERSION 3.13)
project(ECStatic C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(examples)

add_library(ECStatic src/ecstatic.c src/ecstatic.h)
target_include_directories(ECStatic PUBLIC ${CMAKE_SOURCE_DIR}/src)