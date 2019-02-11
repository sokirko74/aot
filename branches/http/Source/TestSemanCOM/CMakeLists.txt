cmake_minimum_required(VERSION 3.5)

project(TestSemanCOM)

add_executable (TestSemanCOM TestSeman.cpp)

add_dependencies(TestSemanCOM  
    Seman
)

