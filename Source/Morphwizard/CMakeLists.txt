declare_cmake_min_version()

project(Morphwizard)



# MFC compiled shared
set(CMAKE_MFC_FLAG 2) 

add_link_options(/ENTRY:wWinMainCRTStartup) 

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /utf-8 /D _AFXDLL /D _UNICODE /D UNICODE " )
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /utf-8 /D _AFXDLL /D _UNICODE /D UNICODE" )
message("CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")

add_executable (${PROJECT_NAME} WIN32
AccentWizardDlg.cpp
ChangeAllDlg.cpp
ChildFrm.cpp
ColorRichEditView.cpp
DlgDelAnc.cpp
DlgLogin.cpp
FileEditCtrl.cpp
GriIni.cpp
InputBox.cpp
ListCtrlToolTip.cpp
MDITabs.cpp
MainFrm.cpp
Morphwizard.cpp
MorphwizardDoc.cpp
MorphwizardView.cpp
OXResizeGadget.cpp
ParadigmDifferences.cpp
ProgressBar.cpp
SLFDocument.cpp
SLFView.cpp
SyntaxColorizer.cpp
stdafx.cpp
tooltip2.cpp
Morphwizard.rc
)


target_link_libraries(${PROJECT_NAME}
    aot_common
    PCRE
    MorphWizardLib
)

CopyExe(${PROJECT_NAME})


