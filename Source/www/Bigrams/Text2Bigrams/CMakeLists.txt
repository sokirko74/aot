declare_cmake_min_version()

project(Text2Bigrams)

add_executable (Text2Bigrams
        Text2Bigrams.cpp
        MergeFiles.cpp
)

target_link_libraries(Text2Bigrams
    GraphanLib
    TrigramLib
)
add_subdirectory (testing)