cmake_minimum_required(VERSION 3.6)

project(SynanLib)

add_library(SynanLib
SentencesCollection.cpp
SyntaxHolder.cpp
)

target_link_libraries(SynanLib
    LemmatizerLib
    GerPostMorph
    MAPostLib
    SimpleGrammarLib
    SynCommonLib
    RusSynanLib
    GerSynanLib
    EngSynanLib
)

add_dependencies(SynanLib Ross)
