#https://github.com/apriorit/FindIDL

# Redistribution and use is allowed under the OSI-approved 3-clause BSD license.
# Copyright (c) 2018 Apriorit Inc. All rights reserved.

set(IDL_FOUND TRUE)

function(add_idl _target _idlfile)
    get_filename_component(IDL_FILE_NAME_WE ${_idlfile} NAME_WE)
    get_filename_component(IDL_FILE_NAME ${_idlfile} NAME)
    set(MIDL_OUTPUT ${IDL_FILE_NAME_WE}_i.h)


    if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        set(MIDL_ARCH win32)
    else()
        set(MIDL_ARCH x64)
    endif()
    
    add_custom_command(
       OUTPUT ${PROJECT_SOURCE_DIR}/../${MIDL_OUTPUT}
       COMMAND "midl.exe" ARGS /${MIDL_ARCH} /env ${MIDL_ARCH} /nologo ${IDL_FILE_NAME} ${MIDL_FLAGS} /I ../.. /h ${MIDL_OUTPUT} 
       DEPENDS ${_idlfile}
       VERBATIM
       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
       #BYPRODUCTS ${MIDL_OUTPUT}
    )

    set(FINDIDL_TARGET ${_target}_gen)
    add_custom_target(
        ${FINDIDL_TARGET} 
        DEPENDS ${PROJECT_SOURCE_DIR}/../${MIDL_OUTPUT} 
        SOURCES ${_idlfile}
    )
    add_library(${_target} INTERFACE)
    add_dependencies(${_target} ${FINDIDL_TARGET})
endfunction()
