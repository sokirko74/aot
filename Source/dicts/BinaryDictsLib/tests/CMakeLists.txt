declare_cmake_min_version()

project(test_binary_dict)

add_executable (${PROJECT_NAME} test_bin_dict.cpp)

target_link_libraries(${PROJECT_NAME} 
    BinaryDictsLib
)


add_test(
        NAME BinDictTest 
        COMMAND test_binary_dict
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

