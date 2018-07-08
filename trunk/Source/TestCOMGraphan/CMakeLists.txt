cmake_minimum_required(VERSION 3.6)

add_executable(TestCOMGraphan
    TestCOMGraphan.cpp
)

add_dependencies(TestCOMGraphan
    graphan
)
