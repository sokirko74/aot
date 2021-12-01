declare_cmake_min_version()

project(WordDatBin)

add_executable (WordDatBin WordDatBin.cpp)

target_link_libraries(WordDatBin 
    LemmatizerBaseLib
)
