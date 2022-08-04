cmake_minimum_required(VERSION 3.21)
project(kechengsizheng C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_executable(kechengsizheng main.c function.c function.h)

add_executable(test_char2 test_char2.c)
add_executable(test_enum test_enum.c)
add_executable(test_run test_run.c)
add_executable(test_array test_array.c function.h function.c)