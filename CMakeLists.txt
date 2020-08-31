project(aot)

set(CMAKE_LEGACY_CYGWIN_WIN32 0)

macro (declare_cmake_min_version)
    cmake_minimum_required(VERSION 3.16)
endmacro(declare_cmake_min_version)

declare_cmake_min_version()

set(BUILD_DICTS 1)  # build dicts by default, disable it in CMakeLists.my if you want
set(BUILD_COM 0)    # do not build COM-objects by default
set(BUILD_HTTP_SERVERS 0) # do not build http-servers by default  (you need to make libevent to build it)
set(BUILD_WINDOWS_GUI 0) # do not build microsoft windows gui by default

if (EXISTS  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.my)
    message ("use " ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.my)
    include (${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.my)
endif()


set (RML $ENV{RML})
#set (INSTALL_BIN_FOLDER $ENV{RML}/Bin)
#set (RML ${CMAKE_CURRENT_SOURCE_DIR})
message ("RML=" ${RML})


set (INSTALL_BIN_FOLDER /Bin) # not "Bin", but "/Bin"

if (NOT RML)
    message (FATAL_ERROR "environment variable RML is not set")
endif (NOT RML)
message ("use environment variable RML = " ${RML} )

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
    message ("build dicts...")
    add_subdirectory (Dicts)
    add_subdirectory (Thes)
endif()

add_subdirectory (Source)

