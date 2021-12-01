project (SynAn)

add_subdirectory (synan_idl)

list (APPEND LIB_DEPENDS 
    SynCommonLib
    SynanLib
    lemmatizer_idl
    graphan_idl
    mapost_idl
        synan_idl
)
 

add_library(${PROJECT_NAME} SHARED
        COMClause.cpp
        COMClauseType.cpp
        COMClauseVariant.cpp
        COMGroup.cpp
        COMHomonym.cpp
        COMSentence.cpp
        COMSentencesCollection.cpp
        COMWord.cpp
        ComRelation.cpp
        ComRelationIterator.cpp
        StdAfx.cpp
        SynAn.cpp
        SynAn.rc
        Unit.cpp
        COMClauseType.rgs
        Clause.rgs
        ClauseVariant.rgs
        Group.rgs
        Homonym.rgs
        Relation.rgs
        RelationsIterator.rgs
        Sentence.rgs
        SentencesCollection.rgs
        SynAn.def
        Unit.rgs
        Word.rgs
)


target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)

CopyAndRegsvr(${PROJECT_NAME})

