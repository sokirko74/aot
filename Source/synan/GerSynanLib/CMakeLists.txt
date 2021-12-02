declare_cmake_min_version()

project(GerSynanLib)

add_library(${PROJECT_NAME}
        GerAnalytical.cpp
        GerBaseRules.cpp
        GerBuildInitialClauses.cpp
        GerClause.cpp
        GerFormatCaller.cpp
        GerGroup.cpp
        GerOborot.cpp
        GerSentence.cpp
        GerSubject.cpp
        GerSyntaxOpt.cpp
        GerThesaurus.cpp
        GerWord.cpp
)

target_link_libraries(${PROJECT_NAME}
    SynCommonLib
)

target_precompile_headers(${PROJECT_NAME} PUBLIC GerSynan.h)