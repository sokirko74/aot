declare_cmake_min_version()

project(TestSynan)

add_executable (${PROJECT_NAME} TestSynan.cpp)

target_link_libraries(${PROJECT_NAME} 
    SynCommonLib
    SynanLib
)
add_subdirectory(test)

