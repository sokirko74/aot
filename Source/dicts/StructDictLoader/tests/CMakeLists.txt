declare_cmake_min_version()

project(StructDictBasic)

add_executable (${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} 
    StructDictLib
)


add_test(
        NAME StructDictBasicTest 
        COMMAND StructDictBasic --test-folder ${PROJECT_SOURCE_DIR}
)

