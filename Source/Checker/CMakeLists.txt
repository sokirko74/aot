cmake_minimum_required(VERSION 3.5)

project(Checker)

add_executable (${PROJECT_NAME} WIN32
Checker.cpp
CheckerDlg.cpp
GriIni.cpp
GriThrPB.cpp
ProceedDlg.cpp
StdAfx.cpp
../common/COM/COMSyntaxHolder.cpp
Checker.rc
)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)


add_dependencies(${PROJECT_NAME} 
    SynAn
    Lemmatizer
    graphan
)

target_link_libraries(${PROJECT_NAME}
    aot_common
)

CopyExe(${PROJECT_NAME})


if (BUILD_DICTS)
    add_dependencies (${PROJECT_NAME} MorphDicts)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" )
