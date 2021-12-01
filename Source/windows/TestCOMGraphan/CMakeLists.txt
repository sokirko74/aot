cmake_minimum_required(VERSION 3.6)

project(TestCOMGraphan)

add_executable(${PROJECT_NAME}
        TestCOMGraphan.cpp
)

add_dependencies(${PROJECT_NAME}
    graphan
)

add_test(
    NAME Graphan_COM_Russian 
    COMMAND TestCOMGraphan test/Russian/text.txt
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME Graphan_COM_Russian_cmp
    COMMAND ${CMAKE_COMMAND} -E compare_files test/Russian/canon.tokens test/Russian/text.txt.tokens
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
