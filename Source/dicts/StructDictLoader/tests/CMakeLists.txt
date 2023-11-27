declare_cmake_min_version()

project(test_struct_dict)

add_executable (${PROJECT_NAME} "test_sruct_dict.cpp")

target_link_libraries(${PROJECT_NAME} 
    StructDictLib
)


add_test(
        NAME StructDictBasicTest 
        COMMAND ${PROJECT_NAME}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

