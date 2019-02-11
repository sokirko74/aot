cmake_minimum_required(VERSION 3.5)

project(SemanDaemon)

add_executable (SemanDaemon Main.cpp)

target_link_libraries(SemanDaemon 
    SemanDmnLib
)

install (TARGETS SemanDaemon
        RUNTIME DESTINATION ${INSTALL_BIN_FOLDER})

