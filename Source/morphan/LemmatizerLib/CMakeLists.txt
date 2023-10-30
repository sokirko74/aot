declare_cmake_min_version()

project(LemmatizerLib)

add_library(LemmatizerLib
        PLMLineCollection.cpp
        GraphanAndMorphanHolder.cpp
        GraphanAndMorphanHolder.h
        PLMLineCollection.h

)

target_link_libraries(LemmatizerLib
    AgramtabLib
    lemmatizer_base_lib
    GraphanLib
    morph_wizard_lib
)

