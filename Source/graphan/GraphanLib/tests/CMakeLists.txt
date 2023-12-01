declare_cmake_min_version()

project(test_graphan)

add_executable (${PROJECT_NAME} test_graphan.cpp )

target_link_libraries(${PROJECT_NAME} 
    GraphanLib
)


add_test(
        NAME GraphanUnitTest 
        COMMAND ${PROJECT_NAME}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

