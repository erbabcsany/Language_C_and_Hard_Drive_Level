cmake_minimum_required(VERSION 4.2)
project(untitled C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES
        main.c
        src/Hadron/BareMetal/coren't/power.c
        src/Hadron/BareMetal/coren't/calcπ.c
        src/Hadron/BareMetal/coren't/temp_history.c
        src/Hadron/BareMetal/coren't/temp_history.h
        src/Hadron/BareMetal/coren't/type.h
        src/Hadron/BareMetal/coren't/type.c
        src/Hadron/BareMetal/coren't/characters.h
        src/Hadron/BareMetal/coren't/puffer.h
        src/Hadron/BareMetal/coren't/push.h
        src/Hadron/BareMetal/coren't/circular/push.h
        src/Hadron/BareMetal/coren't/circular/push.c
)

add_executable(untitled ${SOURCES})

target_compile_options(untitled PRIVATE
        -pedantic
        -Wall
        -Wextra
)

target_link_libraries(untitled m)
