declare_cmake_min_version()

project(morphwizard_qt)

set(CMAKE_AUTOMOC ON)

# Qt is available only as shared libraries, so the global "-static" flag must not be used here
string(REPLACE "-static" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

add_executable (${PROJECT_NAME}
        main.cpp
        MainWindow.cpp
        MainWindow.h
        WizardView.cpp
        WizardView.h
        SlfEditor.cpp
        SlfEditor.h
        DiffViews.cpp
        DiffViews.h
        Dialogs.cpp
        Dialogs.h
        ProgressMeter.cpp
        ProgressMeter.h
        QtUtil.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

target_link_libraries(${PROJECT_NAME}
    morph_wizard_lib
    Qt6::Widgets
)
