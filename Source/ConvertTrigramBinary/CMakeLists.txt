cmake_minimum_required(VERSION 3.6)

project(ConvertTrigramBinary)

add_executable (ConvertTrigramBinary main.cpp)

target_link_libraries(ConvertTrigramBinary
    LemmatizerLib
    TrigramLib
)
