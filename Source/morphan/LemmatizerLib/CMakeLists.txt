declare_cmake_min_version()

project(LemmatizerLib)

add_library(LemmatizerLib
        LemmatizedText.cpp
        LemmatizedText.h
        LemTextCreator.cpp
        LemTextCreator.h
        LemWord.cpp
        LemWord.h
        Homonym.cpp
        Homonym.h
)

target_link_libraries(LemmatizerLib
    AgramtabLib
    lemmatizer_base_lib
    GraphanLib
    morph_wizard_lib
)

