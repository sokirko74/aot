declare_cmake_min_version()

add_library(AgramtabLib
        EngGramTab.cpp
        RusGramTab.cpp
        GerGramTab.cpp
        agramtab_.cpp
        AncodePattern.cpp
)

target_link_libraries(AgramtabLib
        morphology_common
)

add_subdirectory (tests/CheckGramTab)

