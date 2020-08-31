declare_cmake_min_version()

project(Trigram)

add_executable (Trigram main.cpp)

target_link_libraries(Trigram 
    TrigramLib
)
