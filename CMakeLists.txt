cmake_minimum_required(VERSION 3.16)
project(archiver C)

set(CMAKE_C_STANDARD 99)

file(GLOB INCLUDES include/*.h)
file(GLOB SOURCES src/*.c)

add_executable(archiver main.c ${INCLUDES} ${SOURCES})