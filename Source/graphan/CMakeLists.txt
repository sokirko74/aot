project(graphan)

list (APPEND LIB_DEPENDS 
     GraphanLib
)

CompileIDL(${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED
    ComGraphmatFile.cpp
    StdPch.cpp
    graphan.cpp
    graphan.def
    graphan.rc
)


target_link_libraries(${PROJECT_NAME}
    ${LIB_DEPENDS}
)

Regsvr32(${PROJECT_NAME})

