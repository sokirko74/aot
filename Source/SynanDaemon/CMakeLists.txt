declare_cmake_min_version()

project(SynanDaemon)

add_executable (SynanDaemon 
    JVisualSynAnParamBuilder.cpp
    Main.cpp
    SynanDmn.cpp
)

target_link_libraries(SynanDaemon 
    SynanLib
    event
)

message ("install to " ${INSTALL_BIN_FOLDER})
install (TARGETS SynanDaemon
        RUNTIME DESTINATION ${INSTALL_BIN_FOLDER})

