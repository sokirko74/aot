cmake_minimum_required(VERSION 3.5)

project(StructDictLoader)

add_executable (StructDictLoader Main.cpp)

target_link_libraries(StructDictLoader 
    StructDictLib
)
