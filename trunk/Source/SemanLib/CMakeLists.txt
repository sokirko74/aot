cmake_minimum_required(VERSION 3.5)

project(SemanLib)

add_library(SemanLib
AllRossesHolder.cpp
DictUnitInterp.cpp
GenerateNewForm.cpp
HierarchyHolder.cpp
RossHolder.cpp
RussianSynthesis.cpp
SemPattern.cpp
SemStructureBuilder.cpp
SemanHypotGraph.cpp
SemanPattern.cpp
SemanticAnswer.cpp
SemanticClause.cpp
SemanticColloc.cpp
SemanticCommon.cpp
SemanticCompar.cpp
SemanticGraph.cpp
SemanticIdealize.cpp
SemanticLength.cpp
SemanticLexFuncts.cpp
SemanticLexVariant.cpp
SemanticLoc.cpp
SemanticMNA.cpp
SemanticNumeral.cpp
SemanticRusPrimitives.cpp
SemanticStrAn.cpp
SemanticSynRelation.cpp
SemanticThesaurus.cpp
SemanticTime.cpp
SemanticTree.cpp
SemanticWeight.cpp
SemanticsHolder.cpp
TransferSyntaxRelationToSemantics.cpp
TranslatorHolder.cpp
Valency.cpp
stdafx.cpp
)

target_link_libraries(SemanLib
    SynanLib
    Transfer
    BinaryDictsLib
)

add_dependencies (SemanLib 
    BinDicts 
    ThesRosses
    Ross 
    Aoss
    Collocs
    EngCollocs
    TimeRoss) 
