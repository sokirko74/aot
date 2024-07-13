declare_cmake_min_version()

project(MAPostLib)

add_library(MAPostLib
        Fio.cpp
        MAPostInit.cpp
        MAPostMain.cpp
        MapostNumbers.cpp
        MAPostMain.h
        PostMorphInterface.h
)


if (BUILD_DICTS)
    add_dependencies (MAPostLib MorphDicts)
endif()


