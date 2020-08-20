project (Seman)

add_subdirectory (seman_idl)

list (APPEND LIB_DEPENDS 
    SemanLib
    lemmatizer_idl
    graphan_idl
    mapost_idl
    synan_idl
    seman_idl
)
 
add_library(${PROJECT_NAME} SHARED
ComSemNode.cpp
ComSemRelation.cpp
ComSemWord.cpp
SemStructure.cpp
StdAfx.cpp
seman.cpp
seman.def
seman.idl
seman.rc
)


target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)


CopyAndRegsvr(${PROJECT_NAME})
