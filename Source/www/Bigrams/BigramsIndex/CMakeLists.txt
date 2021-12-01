declare_cmake_min_version()

project(BigramsIndex)

add_executable (BigramsIndex BigramsIndex.cpp)

target_link_libraries(BigramsIndex
        aot_common
)
