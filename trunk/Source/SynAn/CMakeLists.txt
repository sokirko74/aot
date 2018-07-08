project (SynAn)

list (APPEND LIB_DEPENDS 
    SynCommonLib
    SynanLib
)
 
CompileIDL(${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED
COMClause.cpp
COMClauseType.cpp
COMClauseVariant.cpp
COMGroup.cpp
COMHomonym.cpp
COMSentence.cpp
COMSentencesCollection.cpp
COMSyntaxHolder.cpp
COMWord.cpp
ComRelation.cpp
ComRelationIterator.cpp
StdAfx.cpp
SynAn.cpp
SynAn.idl
SynAn.rc
Unit.cpp
)


target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)


