cmake_minimum_required(VERSION 3.6)

project(Trigram)

add_executable (Trigram main.cpp)

target_link_libraries(Trigram 
    TrigramLib
)
