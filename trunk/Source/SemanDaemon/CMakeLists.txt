cmake_minimum_required(VERSION 3.5)

project(SemanDaemon)

add_executable (SemanDaemon Main.cpp)

target_link_libraries(SemanDaemon 
    SemanDmnLib
)
