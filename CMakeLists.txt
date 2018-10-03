cmake_minimum_required(VERSION 2.7)

set(CMAKE_C_STANDARD 11)
set(SOURCE_FILES main.c)

set(CMAKE_C_FLAGS "-std=c11 -O2 -Wall -Wextra -Werror -Wno-unused-result")
add_executable(mumsh ${SOURCE_FILES} parse.h para.h execute.h read.c parse.c read.h execute.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=integer")
add_executable(mumsh_memory_check ${SOURCE_FILES})