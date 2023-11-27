declare_cmake_min_version()

project(asp_read)

add_executable (asp_read "asp_read.cpp")

target_link_libraries(asp_read 
    LemmatizerLib
)
add_subdirectory (AspTest)
