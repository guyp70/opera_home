cmake_minimum_required(VERSION 3.5.1)
project(do_not_sleep C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(my_ps)

add_executable(do_not_sleep main.c my_ps/proc_info.h my_ps/proc_info.c)
