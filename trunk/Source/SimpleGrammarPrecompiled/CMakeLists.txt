cmake_minimum_required(VERSION 3.6)

project(SimpleGrammarPrecompiled)

add_executable (SimpleGrammarPrecompiled SimpleGrammarPrecompiled.cpp)

target_link_libraries(SimpleGrammarPrecompiled 
    SimpleGrammarLib
)

add_test(
    NAME GermanGLRTest 
    COMMAND SimpleGrammarPrecompiled --language German --input tests/test.grm --output tests/out.txt
)

add_test(
    NAME GermanGLRTestCompare 
    COMMAND ${CMAKE_COMMAND} -E compare_files tests/canon.log tests/out.txt
)

