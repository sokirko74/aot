declare_cmake_min_version()

project(MorphGen)

add_executable (${PROJECT_NAME}
        MorphGen.cpp
)

target_link_libraries(${PROJECT_NAME} 
    LemmatizerBaseLib
)

add_subdirectory (tests)

