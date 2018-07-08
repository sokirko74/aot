cmake_minimum_required(VERSION 3.5)

project(GerPostMorph)

add_library (GerPostMorph GermanPostMorph.cpp)

target_link_libraries(GerPostMorph 
    LemmatizerLib
    SimpleGrammarLib
)

if (BUILD_DICTS)
    add_dependencies (GerPostMorph 
        GermanGLRSynan
    )
endif()
