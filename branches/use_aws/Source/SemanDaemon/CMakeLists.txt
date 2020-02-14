cmake_minimum_required(VERSION 3.5)

project(SemanDaemon)

add_executable (SemanDaemon 
Main.cpp
translate.cpp
)

target_link_libraries(SemanDaemon
    SemanLib
    event
)

install (TARGETS SemanDaemon
        RUNTIME DESTINATION ${INSTALL_BIN_FOLDER})

