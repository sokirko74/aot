# not tested

cmake_minimum_required(VERSION 3.6)

project(JNIMorphAPI)

add_library(JNIMorphAPI
 JNIMorphAPI.cpp
)

target_compile_definitions(JNIMorphAPI PUBLIC -I $ENV{JAVA_INCLUDES})

target_link_libraries(JNIMorphAPI
    LemmatizerLib
)

