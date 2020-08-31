declare_cmake_min_version()

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

