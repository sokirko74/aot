cmake_minimum_required(VERSION 3.6)

project(GerSynanLib)

add_library(GerSynanLib
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

target_link_libraries(GerSynanLib
    SynCommonLib
)

