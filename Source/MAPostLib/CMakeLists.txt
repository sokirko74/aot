cmake_minimum_required(VERSION 3.5)

project(MAPostLib)

add_library(MAPostLib
    Fio.cpp
    MAPostInit.cpp
    MAPostMain.cpp
    PostLemWord.cpp
    MapostNumbers.cpp
)

target_link_libraries(MAPostLib
    TrigramLib
)

if (BUILD_DICTS)
    add_dependencies (MAPostLib MorphDicts TrigramData)
endif()


