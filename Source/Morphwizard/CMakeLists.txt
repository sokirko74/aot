declare_cmake_min_version()

project(Morphwizard)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)

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
SizeFormView.cpp
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


