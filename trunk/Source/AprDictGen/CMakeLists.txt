cmake_minimum_required(VERSION 3.6)

project(AprDictGen)

add_executable (AprDictGen main.cpp)

target_link_libraries(AprDictGen 
    LemmatizerLib
)
