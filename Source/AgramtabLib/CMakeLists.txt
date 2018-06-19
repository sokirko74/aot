cmake_minimum_required(VERSION 3.6)

add_library(AgramtabLib
    EngGramTab.cpp
    RusGramTab.cpp
    GerGramTab.cpp
    agramtab_.cpp
)

target_link_libraries(AgramtabLib
    aot_common
)

