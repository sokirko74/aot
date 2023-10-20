declare_cmake_min_version()

project(GerPostMorph)

add_library (GerPostMorph GermanPostMorph.cpp
        GermanPostMorph.h)

target_link_libraries(GerPostMorph 
    LemmatizerLib
    SimpleGrammarLib
)

if (BUILD_DICTS)
    add_dependencies (GerPostMorph 
        GermanGLRSynan
    )
endif()
