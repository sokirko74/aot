set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

IF (WIN32)
    # download from https://sourceforge.net/projects/winflexbison/?source=typ_redirect
    SET (WIN_FLEX_FOLDER C:/Users/sokirko/Downloads/winflex)
    SET (FLEX_TOOL ${WIN_FLEX_FOLDER}/win_flex) 
    SET (BISON_TOOL ${WIN_FLEX_FOLDER}/win_bison) 
ELSE (WIN32)
    SET (CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread")
    SET (FLEX_TOOL flex) 
    SET (BISON_TOOL bison) 
ENDIF()


message ("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE MATCHES "Release")
     set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -O2")
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
     set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
     message ("enable_testing")
     enable_testing()
endif(CMAKE_BUILD_TYPE MATCHES "Release")

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
add_subdirectory (SimpleGrammarLib)
add_subdirectory (SimpleGrammarPrecompiled)

add_subdirectory (GenSynDict)
add_subdirectory (EngSynanLib)
add_subdirectory (GerSynanLib)
add_subdirectory (RusSynanLib)
add_subdirectory (SynCommonLib)
add_subdirectory (SynanLib)
add_subdirectory (TestSynan)

add_subdirectory (SemanLib)
add_subdirectory (TestSeman)
add_subdirectory (Transfer)


IF (MSVC)
    message ("compile COM-objects for Microsoft Windows")
    macro(CompileIDL Project)
        file (WRITE dummy.cpp "") # creat empty file it no lib is produced

        add_library(${Project}_TLB
            ${Project}.idl
            dummy.cpp           # without it no lib is produced
        )

        list (APPEND LIB_DEPENDS 
             ${Project}_TLB     # add to dependency list
        )

        add_custom_command(
            TARGET ${Project}_TLB
            POST_BUILD        
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $(IntDir)${Project}.tlb ${CMAKE_SOURCE_DIR}/Bin
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $(IntDir)${Project}.tlb ${PROJECT_SOURCE_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $(IntDir)${Project}.h ${PROJECT_SOURCE_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $(IntDir)${Project}_i.c ${PROJECT_SOURCE_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $(IntDir)${Project}_p.c ${PROJECT_SOURCE_DIR}
            VERBATIM
        )

    endmacro()

    macro(CopyExe Project)
        add_custom_command(
            TARGET ${Project}
            POST_BUILD        
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $(OutDir)${Project}.exe ${CMAKE_SOURCE_DIR}/Bin
            VERBATIM
        )

    endmacro()

    macro(Regsvr32 Project)
        add_custom_command(
            TARGET ${Project}
            POST_BUILD        
            COMMAND echo "skip registering ${CMAKE_SOURCE_DIR}/Bin/${Project}.dll (need admin rights), run $RML/Bin/dllreg.bat manually with admin rights)"
            VERBATIM
        )
    endmacro()

    add_subdirectory (agramtab)
    add_subdirectory (graphan)
    add_subdirectory (MAPost)
    add_subdirectory (SynAn)
    add_subdirectory (StructDict)
    add_subdirectory (Lemmatizer)
    add_subdirectory (Demo)
    add_subdirectory (Seman)
    add_subdirectory (TestCOMGraphan)
    add_subdirectory (TestLemmatizerCOM)
    add_subdirectory (TestSemanCOM)
    add_subdirectory (TestSynanCOM)
    #add_subdirectory (VisualSynan)
    #add_subdirectory (RossDev)
ELSE(WIN32)
    add_subdirectory (SemanDaemon)
    add_subdirectory (SemanDmnLib)
    add_subdirectory (SynanDaemon)
    add_subdirectory (SynanDmnLib)
    add_subdirectory (BigramsCGI)
ENDIF()


