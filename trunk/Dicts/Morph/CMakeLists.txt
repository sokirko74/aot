set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.5)

macro (ConvertMorph name)
    add_custom_target(
        ${name}
        SOURCES morph.bases
    )

    add_custom_command(
        OUTPUT morph.bases
        COMMAND MorphGen project.mwz . 5 3
        DEPENDS MorphGen morphs.mrd project.mwz gramtab.tab
        COMMENT "Convert Morphology ${name}"
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

