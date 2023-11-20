declare_cmake_min_version()

project(test_simple_grammar)

add_executable (${PROJECT_NAME} test_grammar.cpp )

target_link_libraries(${PROJECT_NAME} 
    SimpleGrammarLib
)


add_test(
        NAME SimpleGrammarTest 
        COMMAND test_simple_grammar
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

