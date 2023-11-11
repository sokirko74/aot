declare_cmake_min_version()

add_library(StructDictLib
        ItemsContainer.cpp
        Ross.cpp
        StdRoss.cpp
        TempArticle.cpp
        Domen.cpp
        Field.cpp
        Signat.cpp
        CortegeContainer.cpp
  
        CortegeContainer.h
        Domen.h
        Field.h
        ItemsContainer.h
        
        Ross.h
        Signat.h
        StdRoss.h
        StructDictConsts.h
        TempArticle.h
        TextField.h

)

target_link_libraries(StructDictLib
    aot_common
    RmlThesLib
)

