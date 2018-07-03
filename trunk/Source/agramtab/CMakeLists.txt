cmake_minimum_required(VERSION 3.6)

project (agramtab)

list (APPEND LIB_DEPENDS 
     aot_common AgramtabLib
)
 
CompileIDL(${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED
    ComGramTab.cpp
    StdAfx.cpp
    agramtab.cpp
    agramtab.rc
    EngGramTab.rgs
    GerGramTab.rgs
    RusGramTab.rgs
    agramtab.def
)

target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)


