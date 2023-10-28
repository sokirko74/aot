declare_cmake_min_version()

if (BUILD_COM) 
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
    find_package(IDL REQUIRED)
endif()

macro(CopyAndRegsvr Project)
    add_custom_command(
        TARGET ${Project}
        POST_BUILD        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OutDir}${Project}.dll ${CMAKE_SOURCE_DIR}/Bin
        COMMAND echo "regsvr32 /s ${CMAKE_SOURCE_DIR}/Bin/${Project}.dll (need admin rights))"
        COMMAND regsvr32 /s ${CMAKE_SOURCE_DIR}/Bin/${Project}.dll
        VERBATIM
    )
endmacro()

macro(CopyExe Project)
    add_custom_command(
        TARGET ${Project}
        POST_BUILD        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OutDir}${Project}.exe ${CMAKE_SOURCE_DIR}/Bin
        VERBATIM
    )
endmacro()

if (BUILD_COM)
    message ("compile COM-objects for Microsoft Windows")
    add_subdirectory (agramtab)
    add_subdirectory (graphan)
    add_subdirectory (Lemmatizer)
    add_subdirectory (TestLemmatizerCOM)
endif()

if(BUILD_WINDOWS_GUI)
	        
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        message ("BUILD_WINDOWS_GUI=1")

        #must be compiled 32bit, since we have only 32-bit tcl80.lib
        add_subdirectory (RossDev)
            
        # obsolete mdi tabs cannot be comppiled under 64
        # add_subdirectory (Morphwizard)
    endif()

    add_subdirectory (VisualSynan)
    add_subdirectory (Demo)
    add_subdirectory (Morphwizard)
endif()

