cmake_minimum_required(VERSION 3.6)

project(GenSynDict)

add_executable (GenSynDict main.cpp)

target_link_libraries(GenSynDict 
    LemmatizerLib
)
