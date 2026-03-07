cmake_minimum_required(VERSION 4.1)
project(untitled C)

set(CMAKE_C_STANDARD 90)

add_executable(untitled main.c
        src/Hadron/hadron.c
        src/Hadron/hadron.h
        "src/Fluxus#/kernel.c"
        "src/Fluxus#/kernel.h"
        src/Hadron/lexicon.h
        src/Hadron/macro.h)

