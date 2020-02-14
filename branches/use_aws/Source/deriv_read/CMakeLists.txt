cmake_minimum_required(VERSION 3.5)

project(deriv_read)

add_executable (deriv_read main.cpp)

target_link_libraries(deriv_read 
    LemmatizerLib
)
