project (Seman)

list (APPEND LIB_DEPENDS 
    SemanLib
)
 
CompileIDL(${PROJECT_NAME})

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
