project(Lemmatizer)

add_subdirectory (lemmatizer_idl)

list (APPEND LIB_DEPENDS 
     LemmatizerLib
        lemmatizer_idl
     graphan_idl
)

add_library(${PROJECT_NAME} SHARED
        COMLemmatizerEnglish.rgs
        COMLemmatizerGerman.rgs
        COMLemmatizerRussian.rgs
        COMParadigm.rgs
        COMParadigmCollection.rgs
        ComLemmatizers.cpp
        ComParadigm.cpp
        Lemmatizer.cpp
        Lemmatizer.def
        Lemmatizer.rc
        StdAfx.cpp
)


target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)

#CopyAndRegsvr(${PROJECT_NAME})
