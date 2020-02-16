cmake_minimum_required(VERSION 3.5)

project(BigramsIndex)

add_executable (BigramsIndex BigramsIndex.cpp)

target_link_libraries(BigramsIndex
        aot_common
)
