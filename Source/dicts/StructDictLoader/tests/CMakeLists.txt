declare_cmake_min_version()

project(StructDictBasic)

add_executable (${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} 
    StructDictLib
)


add_test(
        NAME StructDictBasicTest 
        COMMAND StructDictBasic
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

