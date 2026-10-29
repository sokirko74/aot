declare_cmake_min_version()

project(rossdev)

set(CMAKE_AUTOMOC ON)

# Qt is available only as shared libraries, so the global "-static" flag must not be used here
string(REPLACE "-static" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

add_executable (${PROJECT_NAME}
        main.cpp
        MainWindow.cpp
        MainWindow.h
        RossDictionary.cpp
        RossDictionary.h
        WordListWindow.cpp
        WordListWindow.h
        ArticleWindow.cpp
        ArticleWindow.h
        NewArticleWizard.cpp
        NewArticleWizard.h
        Dialogs.cpp
        Dialogs.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

target_link_libraries(${PROJECT_NAME}
    SemanLib
    Transfer
    Qt6::Widgets
)

if (BUILD_DICTS)
    add_dependencies (${PROJECT_NAME} MorphDicts)
endif()
