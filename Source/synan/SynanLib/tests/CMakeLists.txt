declare_cmake_min_version()

project(test_synan)

add_executable (${PROJECT_NAME} test_synan.cpp )

target_link_libraries(${PROJECT_NAME} 
    SynanLib
)


add_test(
        NAME SynanTest 
        COMMAND test_synan
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

