declare_cmake_min_version()

project(FileLem)

add_executable (FileLem FileLem.cpp)

target_link_libraries(FileLem 
    LemmatizerLib
)
