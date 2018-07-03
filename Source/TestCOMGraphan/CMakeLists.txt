cmake_minimum_required(VERSION 3.6)

add_executable(TestCOMGraphan
    stdafx.cpp
    TestCOMGraphan.cpp
)

target_link_libraries(TestCOMGraphan
    graphan
)
