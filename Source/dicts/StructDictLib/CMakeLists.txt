declare_cmake_min_version()

add_library(StructDictLib
        ItemsContainer.cpp
        Ross.cpp
        StdRoss.cpp
        TempArticle.cpp
        Domen.cpp
       "Field.cpp")

target_link_libraries(StructDictLib
    aot_common
    RmlThesLib
)

