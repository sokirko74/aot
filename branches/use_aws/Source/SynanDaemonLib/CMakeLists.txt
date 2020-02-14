cmake_minimum_required(VERSION 3.5)
project(SynanDaemonLib)

add_library(SynanDaemonLib
    JVisualSynAnParamBuilder.cpp
    SynanDmn.cpp
)

target_link_libraries(SynanDaemonLib
        SynanLib
)

