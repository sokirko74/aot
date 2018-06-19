cmake_minimum_required(VERSION 3.5)

project(FileLem)

add_executable (FileLem FileLem.cpp)

target_link_libraries(FileLem 
    LemmatizerLib
)
