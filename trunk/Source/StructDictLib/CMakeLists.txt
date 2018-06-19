cmake_minimum_required(VERSION 3.6)

add_library(StructDictLib
ItemsContainer.cpp
Ross.cpp
StdRoss.cpp
TempArticle.cpp
)

target_link_libraries(StructDictLib
    aot_common
    RmlThesLib
)

