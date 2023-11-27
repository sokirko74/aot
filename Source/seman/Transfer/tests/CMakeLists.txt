declare_cmake_min_version()

project(test_transfer)

add_executable (${PROJECT_NAME} "test_transfer.cpp")

target_link_libraries(${PROJECT_NAME} 
    Transfer  SemanLib
)


add_test(
        NAME TransferTest 
        COMMAND ${PROJECT_NAME}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

