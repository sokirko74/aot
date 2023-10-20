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
        GerClause.h
        GerFormatCaller.h
        GerGroup.h
        GerOborot.h
        GerSentence.h
        GerSynan.h
        GerSyntaxOpt.h
        GerThesaurus.h
        GerWord.h
)

target_link_libraries(${PROJECT_NAME}
    SynCommonLib
)

target_precompile_headers(${PROJECT_NAME} PUBLIC GerSynan.h)