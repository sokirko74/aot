declare_cmake_min_version()

project(RusSynanLib)

add_library(${PROJECT_NAME}
        groups/Adverbs.cpp
        groups/Verbs.cpp
        groups/foreign_term.cpp
        groups/gen_chains.cpp
        groups/noun_adjunct_group.cpp
        groups/prepositions.cpp
        groups/similar_adjs.cpp
        groups/similar_advs.cpp
        groups/similar_common.cpp
        groups/similar_infs.cpp
        groups/similar_noun_groups.cpp
        groups/similar_numerals.cpp
        groups/Selective.cpp
        groups/Gleiche.cpp
        groups/RusFormatCaller.cpp
        groups/RusFormatCaller.h
        groups/names.cpp
        groups/numbers.cpp

        AnalyticalForms.cpp
        BaseRules.cpp
        FormatCallerInit.cpp
        FragCoord.cpp
        FragDisrupt.cpp
        FragPustycha.cpp
        HomonymsKiller.cpp
        NSO.cpp
        Participles.cpp
        RuleForSimilarNounGroupClauses.cpp
        RusBuildInitialClauses.cpp
        RusClause.cpp
        RusGroup.cpp
        RusOborot.cpp
        RusParenthesis.cpp
        RusPlmLine.cpp
        RusSentence.cpp
        RusSyntaxOpt.cpp
        RusThesaurus.cpp
        RusWord.cpp
        StdSynan.cpp
        SynSubjPred.cpp
        SynanDash.cpp
        FormatCallerInit.h
        HomonymsKiller.h
        RusClause.h
        RusGroup.h
        RusOborot.h
        RusPlmLine.h
        RusSentence.h
        RusSyntaxOpt.h
        RusThesaurus.h
        RusWord.h
        StdSynan.h
)


target_link_libraries(${PROJECT_NAME}
    SynCommonLib
)

target_precompile_headers(${PROJECT_NAME} PUBLIC StdSynan.h)
