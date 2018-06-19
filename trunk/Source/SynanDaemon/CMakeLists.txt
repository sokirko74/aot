cmake_minimum_required(VERSION 3.5)

project(SynanDaemon)

add_executable (SynanDaemon Main.cpp)

target_link_libraries(SynanDaemon 
    SynanDmnLib
)
