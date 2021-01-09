declare_cmake_min_version()

project(Morphwizard)

# MFC compiled shared
#add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2) 

add_link_options(/ENTRY:wWinMainCRTStartup) 
set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} /utf-8 /D _AFXDLL /D _UNICODE /D UNICODE " )
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /utf-8 /D _AFXDLL /D _UNICODE /D UNICODE" )

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
Morphwizard.rc
QueryHistory.cpp
AccentModelDiff.cpp
)


target_link_libraries(${PROJECT_NAME}
    aot_common
    PCRE
    MorphWizardLib
)

CopyExe(${PROJECT_NAME})


