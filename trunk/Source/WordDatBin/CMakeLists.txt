cmake_minimum_required(VERSION 3.5)

project(WordDatBin)

add_executable (WordDatBin WordDatBin.cpp)

target_link_libraries(WordDatBin 
    LemmatizerLib
)
