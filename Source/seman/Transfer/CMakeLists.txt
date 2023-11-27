declare_cmake_min_version()

project(Transfer)

add_library(Transfer
        EngAdverbs.cpp
        EngAlg_1.cpp
        EngAlg_2.cpp
        EngAlg_3.cpp
        EngAlg_4.cpp
        EngAlg_5.cpp
        EngArticle.cpp
        EngAtomic.cpp
        EngBinaryTranslate.cpp
        EngEquivs.cpp
        EngHelper.cpp
        EngInterp.cpp
        EngNodeCreate.cpp
        EngNoun.cpp
        EngPrep.cpp
        EngPrimitives.cpp
        EngSynthes.cpp
        EngTerminTranslate.cpp
        EngTimeNodeTranslate.cpp
        EngVerbTense.cpp
        NodeHelper.cpp
        StdAfx.cpp
        TransPrepNoun.cpp
        TransRels.cpp
        TransRules.cpp
        TransTense.cpp
        TranseClause.cpp
        TranslateToEnglish.cpp
        EngBinaryTranslate.h
        EngNumerals.h
        EngPrimitives.h
        EngSemStructure.h
        EngSynthes.h
        EngVerbTense.h
        StdAfx.h

 "eng_word.cpp" "eng_word.h" "verb_tense.h")

add_subdirectory(tests)

target_link_libraries(Transfer
    aot_common
)

target_precompile_headers(${PROJECT_NAME} PUBLIC StdAfx.h)
