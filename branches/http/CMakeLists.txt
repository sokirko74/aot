set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.5)

if (EXISTS  CMakeLists.my)
    include (CMakeLists.my)
endif()

set(BUILD_DICTS 1)  # to disable dicts rebuilding

set (RML $ENV{RML})
set (INSTALL_BIN_FOLDER $ENV{RML}/Bin)

if (NOT RML)
    message (FATAL_ERROR "environment variable RML is not set")
endif (NOT RML)

macro (ConvertStructDict name)
    add_custom_target(
        ${name}
        SOURCES ${PROJECT_SOURCE_DIR}/units.bin
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    add_custom_command(
        OUTPUT ${PROJECT_SOURCE_DIR}/units.bin
        COMMAND StructDictLoader FromTxt ross.txt  .
        DEPENDS StructDictLoader ross.txt
        COMMENT "Convert StructDict for ${name} to the binary format"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        VERBATIM
    )

endmacro(ConvertStructDict)

enable_testing()

if (BUILD_DICTS)
    add_subdirectory (Dicts)
    add_subdirectory (Thes)
endif()

add_subdirectory (Source)

