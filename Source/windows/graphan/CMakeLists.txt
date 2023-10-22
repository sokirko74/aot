project(graphan)

add_subdirectory (graphan_idl)

list (APPEND LIB_DEPENDS 
     GraphanLib graphan_idl
)

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

#CopyAndRegsvr(${PROJECT_NAME})
