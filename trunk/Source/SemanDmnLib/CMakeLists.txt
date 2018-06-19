cmake_minimum_required(VERSION 3.6)

project(SemanDmnLib)

add_library(SemanDmnLib
VisualGraph.cpp
translate.cpp
)

target_link_libraries(SemanDmnLib
    SemanLib
)

