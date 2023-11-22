declare_cmake_min_version()

project(SynanDaemon)

add_executable (SynanDaemon
        JVisualSynAnParamBuilder.cpp
        Main.cpp
        SynanDmn.cpp
        ../../common/http_server.cpp
)

target_link_libraries(SynanDaemon 
    SynanLib
    event
    BigramsLib
    #Libevent
)

message ("install to " ${INSTALL_BIN_FOLDER})
install (TARGETS SynanDaemon
        RUNTIME DESTINATION ${INSTALL_BIN_FOLDER})

