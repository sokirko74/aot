declare_cmake_min_version()

project(Text2Bigrams)

add_executable (Text2Bigrams
        Text2Bigrams.cpp
        MergeFiles.cpp
        MergeFiles.h
)

target_link_libraries(Text2Bigrams
    GraphanLib
)
add_subdirectory (testing)