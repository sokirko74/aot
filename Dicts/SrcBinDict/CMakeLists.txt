project (SrcBinDict)

#if(TARGET MorphDicts) 
#else()
#    message(FATAL_ERROR  "cannot find MorphDicts target, rewrite your cmake files" )
#endif()
set(MorphBinDicts ${PROJECT_SOURCE_DIR}/../Morph/English/morph.bases ${PROJECT_SOURCE_DIR}/../Morph/Russian/morph.bases ${PROJECT_SOURCE_DIR}/../Morph/German/morph.bases)

macro (Convertor BinaryConvertor Source BinData)
    list(APPEND AllBinFiles ${BinData})
    add_custom_command(
        OUTPUT ${BinData}
        COMMAND ${BinaryConvertor} ${Source} ${BinData}
        #DEPENDS ${BinaryConvertor} ${Source} 
        DEPENDS ${Source} ${MorphBinDicts}
        COMMENT "${BinaryConvertor} ${Source} ${BinData}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        VERBATIM
    )
endmacro(Convertor)

Convertor(AprDictGen dict2809.txt ${PROJECT_SOURCE_DIR}/../BinDict/dict.bin)
Convertor(asp_read ASP.txt ${PROJECT_SOURCE_DIR}/../BinDict/asp_dict.bin)
Convertor(deriv_read troiki_suff ${PROJECT_SOURCE_DIR}/../BinDict/deriv_suff.bin)
Convertor(deriv_read troiki_pref ${PROJECT_SOURCE_DIR}/../BinDict/deriv_pref.bin)



macro (GenFreqDictConv Source BinData)
    list(APPEND AllBinFiles ${BinData})
    add_custom_command(
        OUTPUT ${BinData}
        COMMAND GenFreqDict ${Source} ${BinData} shira.txt
        DEPENDS GenFreqDict ${Source} shira.txt ../BinDict/dict.bin ${MorphBinDicts}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "GenFreqDict ${Source} ${BinData} shira.txt" 
        VERBATIM
    )
endmacro(GenFreqDictConv)

GenFreqDictConv (../../Texts/freq_comp.txt ${PROJECT_SOURCE_DIR}/../BinDict/freq_comp.bin)
GenFreqDictConv (../../Texts/freq_fin.txt ${PROJECT_SOURCE_DIR}/../BinDict/freq_fin.bin)
GenFreqDictConv (../../Texts/freq_hud.txt ${PROJECT_SOURCE_DIR}/../BinDict/freq_hud.bin)


add_custom_target( 
    BinDicts
    SOURCES ${AllBinFiles}
)

add_test(
    NAME AspTest
    COMMAND AspTest
)

