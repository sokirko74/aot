declare_cmake_min_version()

add_library(AgramtabLib
    EngGramTab.cpp
    RusGramTab.cpp
    GerGramTab.cpp
    agramtab_.cpp
)

target_link_libraries(AgramtabLib
    aot_common
)

