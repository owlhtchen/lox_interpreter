cmake_minimum_required(VERSION 3.1)

project (lox_interpreter)

set(SOURCES
        src/main.cpp
        src/Scanner.cpp
        )

add_executable(lox_interpreter ${SOURCES})

target_include_directories(lox_interpreter
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include)