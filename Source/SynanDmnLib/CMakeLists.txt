cmake_minimum_required(VERSION 3.6)

project(SynanDmnLib)

add_library(SynanDmnLib
JVisualSynAnParamBuilder.cpp
SynanDmn.cpp
stdafx.cpp
)

target_link_libraries(SynanDmnLib
    SynanLib
)

