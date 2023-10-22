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
        ChildFrm.h
        DlgHom.h
        MainFrm.h
        ReportDoc.h
        SentenceLine.h
        Splash.h
        StdAfx.h
        SynReport.h
        SynanWrapper.h
        VisualSentences.h
        VisualSynan.h
        VisualSynanDoc.h
        VisualSynanView.h
        WaitDlg.h
        WaitThread.h
        
)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /D _UNICODE /D UNICODE" )



target_link_libraries(${PROJECT_NAME} 
    SynCommonLib
    SynanLib
)


if (BUILD_DICTS)
    add_dependencies (${PROJECT_NAME} MorphDicts)
endif()
