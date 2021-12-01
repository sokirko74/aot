declare_cmake_min_version()

project(BinaryDictsClient)

add_executable (BinaryDictsClient Main.cpp)

target_link_libraries(BinaryDictsClient 
    BinaryDictsLib
)
