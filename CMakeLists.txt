set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.5)

set (RML $ENV{RML})

if (NOT RML)
    message (FATAL_ERROR "environment variable RML is not set")
endif (NOT RML)

macro (copy_to_bin target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD        
        ${CMAKE_COMMAND} -E copy_if_different ${target} ${CMAKE_SOURCE_DIR}/Bin
    )
endmacro(copy_to_bin)

#if (WIN32)
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Bin)
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Bin)
#else()
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Bin)
#endif()

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

add_subdirectory (Dicts)
add_subdirectory (Thes)
add_subdirectory (Source)

