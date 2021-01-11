declare_cmake_min_version()

#project(test_tcl)


if (WIN32) 
    include_directories (AFTER ${PROJECT_SOURCE_DIR}/../../tk_source/tk8.0.5/generic ${PROJECT_SOURCE_DIR}/../../tcl_source/tcl8.0.5/generic ${PROJECT_SOURCE_DIR}/../../tk_source/tk8.0.5/xlib)
endif()

add_executable (test_tcl
    test_tcl.cpp
)


target_link_libraries(test_tcl
    aot_common
    ${PROJECT_SOURCE_DIR}/../../tcl_source/tcl80.lib
    ${PROJECT_SOURCE_DIR}/../../tk_source/tk80.lib
)



