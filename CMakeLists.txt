cmake_minimum_required(VERSION 4.2)
project(untitled C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES
        main.c
        src/Hadron/BareMetal/coren't/power.c
)

add_executable(untitled ${SOURCES})

target_compile_options(untitled PRIVATE
        -pedantic
        -Wall
        -Wextra
)

target_link_libraries(untitled m)
