cmake_minimum_required(VERSION 3.6)

project (agramtab)


add_subdirectory (agramtab_idl)


list (APPEND LIB_DEPENDS 
     aot_common AgramtabLib agramtab_idl
)
 

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

CopyAndRegsvr(${PROJECT_NAME})

