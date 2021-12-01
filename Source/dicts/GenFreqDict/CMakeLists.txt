declare_cmake_min_version()

project(GenFreqDict)

add_executable (GenFreqDict main.cpp)

target_link_libraries(GenFreqDict 
    LemmatizerLib
    BinaryDictsLib
)
