project(ConvertToLemma)

add_executable (ConvertToLemma ConvertToLemma.cpp)

target_link_libraries(ConvertToLemma 
    LemmatizerLib
)

if (BUILD_DICTS)
    add_dependencies (ConvertToLemma MorphDicts)
endif()