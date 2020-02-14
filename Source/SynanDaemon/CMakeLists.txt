cmake_minimum_required(VERSION 3.5)

project(SynanDaemon)

add_executable (SynanDaemon
    Main.cpp
)

target_link_libraries(SynanDaemon 
    SynanDaemonLib
    event
)

message ("install to " ${INSTALL_BIN_FOLDER})
install (TARGETS SynanDaemon
        RUNTIME DESTINATION ${INSTALL_BIN_FOLDER})

