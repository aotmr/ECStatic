cmake_minimum_required(VERSION 3.13)
project(ECStatic C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(examples)

add_library(ECStatic src/ecs_core.c src/ecs_core.h src/ecs_utility.h)
target_include_directories(ECStatic PUBLIC ${CMAKE_SOURCE_DIR}/src)