# not tested

declare_cmake_min_version()

project(JNIMorphAPI)

add_library(JNIMorphAPI
        JNIMorphAPI.cpp
)

target_compile_definitions(JNIMorphAPI PUBLIC -I $ENV{JAVA_INCLUDES})

target_link_libraries(JNIMorphAPI
    LemmatizerLib
)

