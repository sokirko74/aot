declare_cmake_min_version()

project(deriv_read)

add_executable (deriv_read main.cpp)

target_link_libraries(deriv_read 
    LemmatizerLib
)
