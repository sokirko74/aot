cmake_minimum_required(VERSION 3.6)

add_executable(TestCOMGraphan
        TestCOMGraphan.cpp
)

add_dependencies(TestCOMGraphan
    graphan
)

add_test(
    NAME Graphan_COM_Russian 
    COMMAND TestCOMGraphan test/Russian/text.txt
    WORKING_DIRECTORY .
)

add_test(
    NAME Graphan_COM_Russian_cmp
    COMMAND ${CMAKE_COMMAND} -E compare_files test/Russian/canon.tokens test/Russian/text.txt.tokens
    WORKING_DIRECTORY .
)
