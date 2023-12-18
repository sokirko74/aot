declare_cmake_min_version()

project(test_seman)

add_executable (${PROJECT_NAME} test_seman.cpp )

target_link_libraries(${PROJECT_NAME} 
    SemanLib
)


add_test(
        NAME SemanTest 
        COMMAND test_seman
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

