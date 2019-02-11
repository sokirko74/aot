cmake_minimum_required(VERSION 3.5)

project(BinaryDictsLib)

add_library(BinaryDictsLib
AspPairDict.cpp
BinaryDictionary.cpp
DerivDict.cpp
FreqDict.cpp
SetOfWordPairs.cpp
SynDictionary.cpp
stdafx.cpp
)

target_link_libraries(BinaryDictsLib
    LemmatizerLib
)

