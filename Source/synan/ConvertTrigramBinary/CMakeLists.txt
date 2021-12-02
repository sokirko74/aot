declare_cmake_min_version()

project(ConvertTrigramBinary)

add_executable (ConvertTrigramBinary main.cpp)

target_link_libraries(ConvertTrigramBinary
    LemmatizerLib
    TrigramLib
)

if (BUILD_DICTS)
    add_dependencies (ConvertTrigramBinary MorphDicts)
endif()
