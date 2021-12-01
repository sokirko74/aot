project(aot_cpp_sources)

set(CMAKE_LEGACY_CYGWIN_WIN32 0)
declare_cmake_min_version()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
set (CMAKE_CXX_STANDARD 17)

IF (WIN32)
    # download from https://sourceforge.net/projects/winflexbison/?source=typ_redirect
    SET (WIN_FLEX_FOLDER ${RML}/external/winflex)
    SET (FLEX_TOOL ${WIN_FLEX_FOLDER}/win_flex) 
    SET (BISON_TOOL ${WIN_FLEX_FOLDER}/win_bison) 
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
ELSE (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
    SET (CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread -lstdc++fs -static")
    SET (FLEX_TOOL flex) 
    SET (BISON_TOOL bison) 
ENDIF()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")



if (BUILD_LEMMATIZER_PYTHON)
  if  (WIN32)
  else()
        message ("use -fPIC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
   endif()
endif()

#json
include_directories(${RML}/Source/contrib/nlohmann)


if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message ("compile debug")
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -D_DEBUG")
    endif()

    enable_testing()
else()
    message ("compile release")
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -O2")
    endif()
endif()


add_subdirectory (tinyxml)
add_subdirectory (common)
add_subdirectory (morph_dict)
add_subdirectory (dicts)
add_subdirectory (graphan)
add_subdirectory (morphan)
add_subdirectory (synan)
add_subdirectory (seman)

IF (MSVC)
    add_subdirectory (windows)
ENDIF()

if(BUILD_HTTP_SERVERS)
    add_subdirectory (www)
endif()

if (BUILD_LEMMATIZER_PYTHON)
   add_subdirectory (morphan/lemmatizer_python)
endif()
