project(Lemmatizer)

list (APPEND LIB_DEPENDS 
     LemmatizerLib
)

CompileIDL(${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED
COMLemmatizerEnglish.rgs
COMLemmatizerGerman.rgs
COMLemmatizerRussian.rgs
COMPLMLineCollection.rgs
COMParadigm.rgs
COMParadigmCollection.rgs
ComLemmatizers.cpp
ComParadigm.cpp
ComPlmLineCollection.cpp
Lemmatizer.cpp
Lemmatizer.def
Lemmatizer.rc
StdAfx.cpp
)


target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)

Regsvr32(${PROJECT_NAME})

