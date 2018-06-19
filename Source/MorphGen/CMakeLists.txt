cmake_minimum_required(VERSION 3.5)

project(MorphGen)

add_executable (MorphGen MorphGen.cpp)

target_link_libraries(MorphGen 
    LemmatizerLib
)
