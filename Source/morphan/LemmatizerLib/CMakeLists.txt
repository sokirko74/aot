declare_cmake_min_version()

project(LemmatizerLib)

add_library(LemmatizerLib
        PLMLineCollection.cpp
        GraphanAndMorphanHolder.cpp
)

target_link_libraries(LemmatizerLib
    AgramtabLib
    LemmatizerBaseLib
    GraphanLib
    MorphWizardLib
)

