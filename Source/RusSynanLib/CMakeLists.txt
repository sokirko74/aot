cmake_minimum_required(VERSION 3.6)

project(RusSynanLib)

add_library(RusSynanLib
Adverbs.cpp
AnalyticalForms.cpp
BaseRules.cpp
FormatCallerInit.cpp
FragCoord.cpp
FragDisrupt.cpp
FragPustycha.cpp
Gleiche.cpp
HomonymsKiller.cpp
NSO.cpp
Names.cpp
Numbers.cpp
Participles.cpp
RuleForSimilarNounGroupClauses.cpp
RusBuildInitialClauses.cpp
RusClause.cpp
RusFormatCaller.cpp
RusGroup.cpp
RusOborot.cpp
RusParenthesis.cpp
RusPlmLine.cpp
RusSentence.cpp
RusSyntaxOpt.cpp
RusThesaurus.cpp
RusWord.cpp
Selective.cpp
StdSynan.cpp
SynSubjPred.cpp
SynanDash.cpp
Verbs.cpp
foreign_term.cpp
gen_chains.cpp
noun_adjunct_group.cpp
prepositions.cpp
similar_adjs.cpp
similar_advs.cpp
similar_common.cpp
similar_infs.cpp
similar_noun_groups.cpp
similar_numerals.cpp
)


target_link_libraries(RusSynanLib
    SynCommonLib
)

