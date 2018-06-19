cmake_minimum_required(VERSION 3.6)

project(asp_read)

add_executable (asp_read main.cpp)

target_link_libraries(asp_read 
    LemmatizerLib
)
