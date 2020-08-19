project(MAPost)

add_subdirectory (mapost_idl)

list (APPEND LIB_DEPENDS 
    MAPostLib
    GerPostMorph
    lemmatizer_idl
    graphan_idl
    mapost_idl
)


add_library(${PROJECT_NAME} SHARED
    MAPost.cpp
    MAPost.def
    MAPost.rc
    MAPostCOM.cpp
    StdAfx.cpp
)


target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)

CopyAndRegsvr(${PROJECT_NAME})
