cmake_minimum_required(VERSION 3.6)

project(BigramsCGI)

add_executable (BigramsCGI Main.cpp)

target_link_libraries(BigramsCGI 
    aot_common
)
