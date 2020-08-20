cmake_minimum_required(VERSION 3.6)

add_executable(TestCOMGraphan
    TestCOMGraphan.cpp
)

add_dependencies(TestCOMGraphan
    graphan
)

add_test(
    NAME RussianCOMGraphan 
    COMMAND TestCOMGraphan test/Russian/text.txt
    WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME RussianCOMGraphanCompare 
    COMMAND ${CMAKE_COMMAND} -E compare_files  test/Russian/canon.tokens test/Russian/text.txt.tokens
    WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}
)
