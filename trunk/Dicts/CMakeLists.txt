set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.6)

macro (ConvertStructDict name)
    add_custom_target(
        ${name}
        SOURCES units.bin
    )

    add_custom_command(
        OUTPUT units.bin
        COMMAND StructDictLoader FromTxt ross.txt  .
        DEPENDS StructDictLoader ross.txt
        COMMENT "Convert StructDict for ${name} to the binary format"
        VERBATIM
    )

endmacro(ConvertStructDict)

macro (CompileSimpleGrammar name grmFile tableFile)
    add_custom_target( 
        ${name}
        SOURCES ${tableFile}                     
    )

    add_custom_command(
        OUTPUT ${tableFile}
        COMMAND SimpleGrammarPrecompiled --language German --input ${grmFile} --output -
        DEPENDS SimpleGrammarPrecompiled ${grmFile}
        COMMENT "Compile ${grmFile} "
        VERBATIM
    )

endmacro(CompileSimpleGrammar)



add_subdirectory (Obor)
add_subdirectory (GerObor)
add_subdirectory (EngObor)

add_subdirectory (Aoss)
add_subdirectory (Ross)
add_subdirectory (Collocs)
add_subdirectory (EngCollocs)
add_subdirectory (TimeRoss)

add_subdirectory (Morph)

add_subdirectory (Trigram)

add_subdirectory (GerSynan)
add_subdirectory (SimpleGrammar)

add_subdirectory (SrcBinDict)

add_custom_target ( 
    GermanGLRSynan
    DEPENDS GerNPGLRSynan ExampleGLRSynan PersonGLRSynan
)


