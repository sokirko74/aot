cmake_minimum_required(VERSION 3.5)

project(demo)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)

add_executable (demo WIN32
    Splash.cpp
    StdAfx.cpp
    demo.cpp
    demo.rc
    demoDlg.cpp
)

target_link_libraries(demo 
    Lemmatizer
)

add_dependencies (demo MorphDicts)
