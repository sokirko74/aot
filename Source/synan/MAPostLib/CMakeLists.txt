declare_cmake_min_version()

project(MAPostLib)

add_library(MAPostLib
        Fio.cpp
        MAPostInit.cpp
        MAPostMain.cpp
        PostLemWord.cpp
        MapostNumbers.cpp
        MAPostMain.h
        PostMorphInterface.h
        PostLemWord.h
)


if (BUILD_DICTS)
    add_dependencies (MAPostLib MorphDicts TrigramData)
endif()


