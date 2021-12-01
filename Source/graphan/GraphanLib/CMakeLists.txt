declare_cmake_min_version()

project(GraphanLib)

add_library(GraphanLib
        C_desc.cpp
        Consent.cpp
        Cs_hiera.cpp
        GraphanDicts.cpp
        GraphmatFile.cpp
        HtmlConv.cpp
        Nc_desc.cpp
        Oborots.cpp
        Sentences.cpp
        StdAfx.cpp
        UnitHolder.cpp
        Writegra.cpp
        abbrev.cpp
        graline.cpp
)

target_link_libraries(GraphanLib
    aot_common
    StructDictLib
    RmlThesLib
)

if (BUILD_DICTS)
    add_dependencies (GraphanLib Obor GerObor EngObor)
endif()
