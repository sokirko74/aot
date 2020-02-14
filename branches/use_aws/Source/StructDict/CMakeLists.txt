project(StructDict)

list (APPEND LIB_DEPENDS 
     StructDictLib 
)

CompileIDL(${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED
    ComDictionary.cpp
    ComDomen.cpp
    ComField.cpp
    ComSignat.cpp
    ComTempArticle.cpp
    ComTextField.cpp
    StdPch.cpp
    StructDict.cpp
    StructDict.def
    ComDictionary.rgs
    ComDomen.rgs
    ComField.rgs
    ComSignat.rgs
    ComTempArticle.rgs
    ComTextField.rgs
)


target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)
