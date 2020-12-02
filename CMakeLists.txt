cmake_minimum_required(VERSION 3.7)
project(tests LANGUAGES C)

set(CMAKE_C_STANDARD 99)

add_executable(converter src/converter/converter.c src/converter/converter_func.c )
add_executable(comparer src/comparer/comparer.c src/comparer/comparer_func.c)

