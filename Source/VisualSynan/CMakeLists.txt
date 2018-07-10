cmake_minimum_required(VERSION 3.5)

project(VisualSynan)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)

add_executable (${PROJECT_NAME} WIN32
ChildFrm.cpp
DlgHom.cpp
MainFrm.cpp
ReportDoc.cpp
SentenceLine.cpp
Splash.cpp
StdAfx.cpp
SynReport.cpp
VisualClause.cpp
VisualGroup.cpp
VisualGroups.cpp
VisualSentence.cpp
VisualSentences.cpp
VisualSynan.cpp
VisualSynan.rc
VisualSynanDoc.cpp
VisualSynanView.cpp
VisualWord.cpp
WaitDlg.cpp
WaitThread.cpp
../common/COM/COMSyntaxHolder.cpp
)

add_dependencies(${PROJECT_NAME} 
    SynAn
)

target_link_libraries(${PROJECT_NAME}
    aot_common
)

CopyExe(${PROJECT_NAME})


if (BUILD_DICTS)
    add_dependencies (${PROJECT_NAME} MorphDicts)
endif()
