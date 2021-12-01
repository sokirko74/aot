declare_cmake_min_version()

project(BinaryDictsLib)

add_library(BinaryDictsLib
        AspPairDict.cpp
        BinaryDictionary.cpp
        DerivDict.cpp
        FreqDict.cpp
        SetOfWordPairs.cpp
        stdafx.cpp
)

target_link_libraries(BinaryDictsLib
    LemmatizerLib
)

