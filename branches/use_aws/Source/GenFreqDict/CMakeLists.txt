cmake_minimum_required(VERSION 3.5)

project(GenFreqDict)

add_executable (GenFreqDict main.cpp)

target_link_libraries(GenFreqDict 
    LemmatizerLib
    BinaryDictsLib
)
