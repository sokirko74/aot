declare_cmake_min_version()

project(test_asp_dict)

add_executable (${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} 
    BinaryDictsLib
)

add_test(
        NAME AspDictTest 
        COMMAND ${PROJECT_NAME}
)

