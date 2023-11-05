declare_cmake_min_version()

project(AprDictGen)

add_executable (AprDictGen main.cpp)

target_link_libraries(AprDictGen
    LemmatizerLib
        BinaryDictsLib
)

if (BUILD_DICTS)
    add_dependencies (AprDictGen MorphDicts)
endif()
