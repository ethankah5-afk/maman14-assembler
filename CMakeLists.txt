cmake_minimum_required(VERSION 3.28)
project(assemblerProject C)

set(CMAKE_C_STANDARD 90)

add_executable(assemblerProject
        main_struct.h
        main_struct.c
        conversion.c
        pre_proc.c
        pre_proc.h
        first_pass.c
        first_pass.h
        assembler.c)
