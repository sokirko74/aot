declare_cmake_min_version()

project(RossDev)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)

if (WIN32) 
    include_directories (AFTER ../tk_source/tk8.0.5/generic ../tcl_source/tcl8.0.5/generic ../tk_source/tk8.0.5/xlib)
endif()

add_executable (${PROJECT_NAME} WIN32
        ArticleDoc.cpp
        ArticleView.cpp
        
        ChildFrm.cpp
        CntrItem.cpp
        FieldList.cpp
        
        FindByStrDlg.cpp
        FindWrongRefs.cpp
        FldScroll.cpp
        
        
        Hierarchy.cpp
        HierarchyDoc.cpp
        
        InputBox.cpp
        MainFrm.cpp
        NewArticleWizard.cpp
        NewRefArticles.cpp
        NewTextEntry.cpp
        OXResizeGadget.cpp
        PocketDoc.cpp
        PocketForm.cpp
        PosChoicer.cpp
        ProgressForm.cpp
        ReportDoc.cpp
        ReportView.cpp
        RossDev.cpp
        RossDev.rc
        RossDev.rgs
        RossDoc.cpp
        
        SemanticStrDoc.cpp
        SemanticStrView.cpp
        
        SizeFormView.cpp
        Splash.cpp
        StdAfx.cpp
        SyntaxColorizer.cpp
        Translations.cpp
        
        WordList.cpp
        font.cpp
        picture.cpp
        rowcursor.cpp
 )


target_link_libraries(${PROJECT_NAME}
    aot_common
    StructDictLib
    SynCommonLib
    SynanLib
    SemanLib
    Transfer
    ${PROJECT_SOURCE_DIR}/../tcl_source/tcl80.lib
    ${PROJECT_SOURCE_DIR}/../tk_source/tk80.lib
)

add_subdirectory("tests")

target_precompile_headers(${PROJECT_NAME} PUBLIC StdAfx.h)

CopyExe(${PROJECT_NAME})

