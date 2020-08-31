project(aot_cpp_sources)

set(CMAKE_LEGACY_CYGWIN_WIN32 0)
declare_cmake_min_version()

set (CMAKE_CXX_STANDARD 17)

IF (WIN32)
    # download from https://sourceforge.net/projects/winflexbison/?source=typ_redirect
    SET (WIN_FLEX_FOLDER ${RML}/external/winflex)
    SET (FLEX_TOOL ${WIN_FLEX_FOLDER}/win_flex) 
    SET (BISON_TOOL ${WIN_FLEX_FOLDER}/win_bison) 
ELSE (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
    SET (CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread -lstdc++fs ")
    SET (FLEX_TOOL flex) 
    SET (BISON_TOOL bison) 
ENDIF()

#http server 
if(BUILD_HTTP_SERVERS)
    message ("include_directories contrib/libevent/include" )
    include_directories(contrib/libevent/include)
    find_package(libevent REQUIRED PATHS contrib/libevent)
ENDIF()

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


add_subdirectory (contrib/iconv_subset)

add_subdirectory (tinyxml)
add_subdirectory (common)
add_subdirectory (PCRE)

add_subdirectory (StructDictLib)
add_subdirectory (StructDictLoader)
add_subdirectory (AgramtabLib)
add_subdirectory (GraphanLib)
add_subdirectory (RmlThesLib)
add_subdirectory (GraphmatThick)
add_subdirectory (MorphWizardLib)
add_subdirectory (LemmatizerLib)
add_subdirectory (TestLem)
add_subdirectory (FileLem)
add_subdirectory (MorphGen)

if ($JAVA_INCLUDES)  # not tested under cmake
    add_subdirectory (JNIMorphAPI)
    add_subdirectory (JNIMorphAPITest)
endif()

add_subdirectory (Bigrams)
add_subdirectory (BinaryDictsClient)
add_subdirectory (BinaryDictsLib)
add_subdirectory (AprDictGen)
add_subdirectory (asp_read)
add_subdirectory (GenFreqDict)
add_subdirectory (StatDatBin)
add_subdirectory (WordDatBin)
add_subdirectory (deriv_read)
add_subdirectory (ConvertTrigramBinary)
add_subdirectory (Trigram)
add_subdirectory (TrigramLib)
add_subdirectory (TrigramRusCorporaXml)
add_subdirectory (CheckGramTab)

add_subdirectory (GerPostMorph)
add_subdirectory (MAPostLib)
add_subdirectory (TestMapost)
add_subdirectory (SimpleGrammarLib)
add_subdirectory (SimpleGrammarPrecompiled)

add_subdirectory (EngSynanLib)
add_subdirectory (GerSynanLib)
add_subdirectory (RusSynanLib)
add_subdirectory (SynCommonLib)
add_subdirectory (SynanLib)
add_subdirectory (TestSynan)

add_subdirectory (SemanLib)
add_subdirectory (TestSeman)
add_subdirectory (Transfer)

if(BUILD_HTTP_SERVERS)
    add_subdirectory (SynanDaemon)
    add_subdirectory (SemanDaemon)
endif()

if (BUILD_COM) 
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
    find_package(IDL REQUIRED)
endif()


IF (MSVC)
    macro(CopyAndRegsvr Project)
        add_custom_command(
            TARGET ${Project}
            POST_BUILD        
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OutDir}${Project}.dll ${CMAKE_SOURCE_DIR}/Bin
            COMMAND echo "regsvr32 /s ${CMAKE_SOURCE_DIR}/Bin/${Project}.dll (need admin rights))"
            COMMAND regsvr32 /s ${CMAKE_SOURCE_DIR}/Bin/${Project}.dll
            VERBATIM
        )
    endmacro()

    macro(CopyExe Project)
        add_custom_command(
            TARGET ${Project}
            POST_BUILD        
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OutDir}${Project}.exe ${CMAKE_SOURCE_DIR}/Bin
            VERBATIM
        )
    endmacro()

    if (BUILD_COM)
        message ("compile COM-objects for Microsoft Windows")
        add_subdirectory (agramtab)
        add_subdirectory (graphan)
        add_subdirectory (Lemmatizer)
        add_subdirectory (MAPost)
        add_subdirectory (SynAn)
        add_subdirectory (Seman)
        add_subdirectory (TestCOMGraphan)
        add_subdirectory (TestLemmatizerCOM)
        add_subdirectory (TestSemanCOM)
        add_subdirectory (TestSynanCOM)
        if(BUILD_WINDOWS_GUI)
            add_subdirectory (VisualSynan)
            add_subdirectory (Demo)
            add_subdirectory (Checker)
        endif()
    endif()

    if(BUILD_WINDOWS_GUI)
        #must be compiled 32bit, since we have only 32-bit tcl80.lib
        if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
            add_subdirectory (RossDev)
            add_subdirectory (Morphwizard)
        endif()
    endif()

ENDIF()

