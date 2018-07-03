project(ConvertToLemma)

add_executable (ConvertToLemma ConvertToLemma.cpp)

target_link_libraries(ConvertToLemma 
    LemmatizerLib
)

add_dependencies (ConvertToLemma MorphDicts)