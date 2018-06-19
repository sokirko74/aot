set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.5)


macro (Convertor BinaryConvertor Source BinData)
    list(APPEND AllBinFiles ${BinData})
    add_custom_command(
        OUTPUT ${BinData}
        COMMAND ${BinaryConvertor} ${Source} ${BinData}
        DEPENDS ${BinaryConvertor} ${Source}
        COMMENT "${BinaryConvertor} ${Source} ${BinData}"
        VERBATIM
    )

endmacro(Convertor)

Convertor(AprDictGen dict2809.txt ../BinDict/dict.bin)
Convertor(asp_read ASP.txt ../BinDict/asp_dict.bin)
Convertor(deriv_read troiki_suff ../BinDict/deriv_suff.bin)
Convertor(deriv_read troiki_pref ../BinDict/deriv_pref.bin)
Convertor(GenSynDict synonyms.txt ../BinDict/synonyms.bin)


macro (GenFreqDictConv Source BinData)
    list(APPEND AllBinFiles ${BinData})
    add_custom_command(
        OUTPUT ${BinData}
        COMMAND GenFreqDict ${Source} ${BinData} shira.txt
        DEPENDS GenFreqDict ${Source} shira.txt ../BinDict/dict.bin
        COMMENT "GenFreqDict ${Source} ${BinData} shira.txt" 
        VERBATIM
    )
endmacro(GenFreqDictConv)

GenFreqDictConv (../../Texts/freq_comp.txt ../BinDict/freq_comp.bin)
GenFreqDictConv (../../Texts/freq_fin.txt ../BinDict/freq_fin.bin)
GenFreqDictConv (../../Texts/freq_hud.txt ../BinDict/freq_hud.bin)


macro (StatDatBinRun lang Source Topic)
    set (BinData ../Morph/${lang}/${Topic}homoweight.bin)
    list(APPEND AllBinFiles ${BinData})
    add_custom_command(
        OUTPUT ${BinData}
        COMMAND StatDatBin --input ${Source} --language ${lang} --output ${BinData}
        DEPENDS StatDatBin ${Source}
        COMMENT "Convert ${Source} to the binary format"
        VERBATIM
    )
endmacro(StatDatBinRun)

macro (WordDatBinRun lang Source Topic)
    set (BinData ../Morph/${lang}/${Topic}wordweight.bin)
    list(APPEND AllBinFiles ${BinData})
    add_custom_command(
        OUTPUT ${BinData}
        COMMAND WordDatBin --input ${Source} --language ${lang} --output ${BinData}
        DEPENDS WordDatBin ${Source}
        COMMENT "Convert ${Source} to the binary format"
        VERBATIM
    )
endmacro(WordDatBinRun)

foreach (Topic l c f)
    StatDatBinRun (Russian StatData.rus ${Topic} ) 
    StatDatBinRun (German  StatData.ger ${Topic} ) 
    WordDatBinRun (Russian WordData.txt ${Topic} )
endforeach (Topic)


add_custom_target( 
    BinDicts
    SOURCES ${AllBinFiles}
)



