cmake_minimum_required(VERSION 3.5)

project(BigramsTest)

add_executable (BigramsTest main.cpp)

target_link_libraries(BigramsTest
        aot_common
)
