declare_cmake_min_version()

project(SynanLib)

add_library(SynanLib
        SentencesCollection.cpp
        SyntaxHolder.cpp
        SentencesCollection.h
        SyntaxHolder.h
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

if (BUILD_DICTS)
    add_dependencies(SynanLib Ross)
endif()

add_subdirectory(tests)
