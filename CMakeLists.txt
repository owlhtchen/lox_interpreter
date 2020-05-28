cmake_minimum_required(VERSION 3.15)

project (lox_interpreter)

set(SOURCES
        src/main.cpp
        src/Scanner.cpp
        src/Expr.cpp
        src/CodeGenerator.cpp
        src/Chunk.cpp
        src/Error.cpp
        src/Parser.cpp
        src/AstPrinter.cpp
        src/VM.cpp
        src/debug.cpp
        )

add_executable(lox_interpreter ${SOURCES})

target_compile_features(lox_interpreter PRIVATE cxx_std_17)

target_include_directories(lox_interpreter
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include)