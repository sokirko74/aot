cmake_minimum_required(VERSION 3.5)

project(TrigramLib)

add_library(TrigramLib
NGramFile.cpp
ReadLexFile.cpp
TagSet.cpp
TrigramLemmatizer.cpp
TrigramModel.cpp
TrigramSerialize.cpp
Viterbi.cpp
ViterbiInfo.cpp
stdafx.cpp
)

target_compile_definitions(PCRE PUBLIC -DUSE_TRIGRAM_LEMMATIZER)

target_link_libraries(TrigramLib
    LemmatizerLib
)

