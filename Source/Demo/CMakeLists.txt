declare_cmake_min_version()

project(demo)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /D _UNICODE /D UNICODE" )
add_link_options(/ENTRY:wWinMainCRTStartup) 

add_executable (${PROJECT_NAME} WIN32
    Splash.cpp
    StdAfx.cpp
    demo.cpp
    demo.rc
    demoDlg.cpp
    ../common/COM/windows_unicode.cpp
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
