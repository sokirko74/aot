cmake_minimum_required(VERSION 3.6)

project(StatDatBin)

add_executable (StatDatBin StatDatBin.cpp)

target_link_libraries(StatDatBin 
    LemmatizerLib
)
