cmake_minimum_required(VERSION 3.6)

project(GerPostMorph)

add_library (GerPostMorph GermanPostMorph.cpp)

target_link_libraries(GerPostMorph 
    LemmatizerLib
)

add_dependencies (GerPostMorph 
    GermanGLRSynan
)
