cmake_minimum_required(VERSION 3.5)

project(TestSeman)

add_executable (TestSeman TestSeman.cpp)

target_link_libraries(TestSeman 
    SemanLib
)

add_test(
    NAME Seman 
    COMMAND TestSeman --input test/Russian/test.txt --output test/Russian/out.txt
    WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
)

add_test (
    NAME Seman_cmp 
    COMMAND ${CMAKE_COMMAND} -E compare_files  test/Russian/canon.txt test/Russian/out.txt
    WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
)
