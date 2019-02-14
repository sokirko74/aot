cmake_minimum_required(VERSION 3.5)

project(AprDictGen)

add_executable (AprDictGen main.cpp)

target_link_libraries(AprDictGen 
    LemmatizerLib
)

if (BUILD_DICTS)
    add_dependencies (AprDictGen MorphDicts)
endif()
