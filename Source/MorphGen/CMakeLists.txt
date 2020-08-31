declare_cmake_min_version()

project(MorphGen)

add_executable (MorphGen MorphGen.cpp)

target_link_libraries(MorphGen 
    LemmatizerLib
)
