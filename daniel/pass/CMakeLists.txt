cmake_minimum_required(VERSION 3.16)
project(pass)
set(CMAKE_C_STANDARD 11)
set(SOURCE_FILES main.c ps.h ps.c)

add_executable(Pass ${SOURCE_FILES})