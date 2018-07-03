set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.5)

macro (ConvertMorph langua)
    set( target_name "${langua}Morph")
    set( folder  ${PROJECT_SOURCE_DIR}/Dicts/Morph/${langua} )
    add_custom_target(
        ${target_name}
        SOURCES morph.bases
        WORKING_DIRECTORY ${folder}
    )

    add_custom_command(
        OUTPUT ${folder}/morph.bases
        COMMAND MorphGen --input project.mwz --output-folder . --postfix-len 5 --min-freq 3
        DEPENDS MorphGen morphs.mrd project.mwz gramtab.tab
        COMMENT "Convert Morphology ${langua}"                 
        WORKING_DIRECTORY ${folder}
        VERBATIM
    )

endmacro(ConvertMorph)


add_subdirectory (Russian)
add_subdirectory (English)
add_subdirectory (German)

add_custom_target(
    MorphDicts
    DEPENDS  EnglishMorph RussianMorph GermanMorph
)

