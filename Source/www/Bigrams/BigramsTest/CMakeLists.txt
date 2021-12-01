declare_cmake_min_version()

project(BigramsTest)

add_executable (BigramsTest main.cpp)

target_link_libraries(BigramsTest
        aot_common
)
