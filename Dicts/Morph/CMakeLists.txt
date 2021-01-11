set(CMAKE_LEGACY_CYGWIN_WIN32 0)
declare_cmake_min_version()

project(MorphAllLanguages)

macro (RunMorphGen langua)
    set( folder  ${PROJECT_SOURCE_DIR}/${langua} )

    add_custom_target(
        "MorphGen_on_${langua}"
         SOURCES ${folder}/morph.bases ${folder}/morph.annot ${folder}/morph.forms_autom
    )

    add_custom_command(
        OUTPUT ${folder}/morph.bases ${folder}/morph.annot ${folder}/morph.forms_autom
        COMMAND MorphGen --input project.mwz --output-folder . --postfix-len 5 --min-freq 3
        DEPENDS MorphGen ${folder}/morphs.mrd ${folder}/project.mwz ${folder}/gramtab.tab
        COMMENT "Convert Morphology ${langua}"                 
        WORKING_DIRECTORY ${folder}
        VERBATIM
    )
endmacro()


macro (StatDatBinRun langua Topic)
    set (Src ${PROJECT_SOURCE_DIR}/../SrcBinDict/StatData.${langua})
    set (Trg ${PROJECT_SOURCE_DIR}/${langua}/${Topic}homoweight.bin)
    add_custom_command(
        OUTPUT ${Trg}
        COMMAND StatDatBin --input ${Src} --language ${langua} --output ${Trg}
        DEPENDS StatDatBin ${Src} "${PROJECT_SOURCE_DIR}/${langua}/morph.bases"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Convert ${Src} to the binary format"
        VERBATIM
    )
endmacro()

macro (WordDatBinRun langua Topic)
    set (Src ${PROJECT_SOURCE_DIR}/../SrcBinDict/WordData.${langua})
    set (Trg ${PROJECT_SOURCE_DIR}/${langua}/${Topic}wordweight.bin)
    add_custom_command(
        OUTPUT ${Trg}
        COMMAND WordDatBin --input ${Src} --language ${langua} --output ${Trg}
        DEPENDS WordDatBin ${Src} "${PROJECT_SOURCE_DIR}/${langua}/morph.bases"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Convert ${Source} to the binary format"
        VERBATIM
    )
endmacro()


macro (CreateAllMorphBinFiles langua)
    set( folder  ${PROJECT_SOURCE_DIR}/${langua} )    
    RunMorphGen (${langua})
    
    foreach (Topic l c f)
        StatDatBinRun ( ${langua} ${Topic} ) 
        WordDatBinRun ( ${langua} ${Topic} )
    endforeach ()

    add_custom_target(
        "${langua}_Morph"
        SOURCES ${folder}/morph.bases ${folder}/chomoweight.bin ${folder}/lhomoweight.bin ${folder}/fhomoweight.bin ${folder}/cwordweight.bin ${folder}/lwordweight.bin ${folder}/fwordweight.bin
        WORKING_DIRECTORY ${folder}
    )
endmacro()

CreateAllMorphBinFiles(Russian)
CreateAllMorphBinFiles(German)
CreateAllMorphBinFiles(English)

add_custom_target(
    MorphDicts
    DEPENDS  Russian_Morph German_Morph English_Morph
)

