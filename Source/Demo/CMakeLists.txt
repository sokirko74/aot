cmake_minimum_required(VERSION 3.5)

project(demo)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)

add_executable (${PROJECT_NAME} WIN32
    Splash.cpp
    StdAfx.cpp
    demo.cpp
    demo.rc
    demoDlg.cpp
)

add_dependencies(${PROJECT_NAME} 
    Lemmatizer
)

target_link_libraries(${PROJECT_NAME}
    aot_common
)

CopyExe(${PROJECT_NAME})


if (BUILD_DICTS)
    add_dependencies (${PROJECT_NAME} MorphDicts)
endif()
