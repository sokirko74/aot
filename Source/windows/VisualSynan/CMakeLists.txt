declare_cmake_min_version()

project(VisualSynan)

add_link_options(/ENTRY:wWinMainCRTStartup) 

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
        ../../common/COM/COMSyntaxHolder.cpp
)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /D _UNICODE /D UNICODE" )


add_dependencies(${PROJECT_NAME} 
    agramtab
    SynAn
    Lemmatizer
    graphan
    MAPost
)

target_link_libraries(${PROJECT_NAME}
    aot_common
)

CopyExe(${PROJECT_NAME})


if (BUILD_DICTS)
    add_dependencies (${PROJECT_NAME} MorphDicts)
endif()
