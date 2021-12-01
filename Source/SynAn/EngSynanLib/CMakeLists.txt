declare_cmake_min_version()

project(EngSynanLib)

add_library(EngSynanLib
        EngFormatCaller.cpp
        EngOborDic.cpp
        EngSentence.cpp
        EngSyntaxOpt.cpp
        stdafx.cpp
)

target_link_libraries(EngSynanLib
    SynCommonLib
)

