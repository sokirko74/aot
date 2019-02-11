cmake_minimum_required(VERSION 3.5)

project(BinaryDictsClient)

add_executable (BinaryDictsClient Main.cpp)

target_link_libraries(BinaryDictsClient 
    BinaryDictsLib
)
