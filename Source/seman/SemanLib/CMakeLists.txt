declare_cmake_min_version()

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
        VisualGraph.cpp
        SemanticWeightComponents.cpp
)

target_link_libraries(SemanLib
    SynanLib
    Transfer
    BinaryDictsLib
)

target_precompile_headers(${PROJECT_NAME} PUBLIC stdafx.h)

if (BUILD_DICTS)
    add_dependencies (SemanLib 
        BinDicts 
        ThesRosses
        Ross 
        Aoss
        Collocs
        EngCollocs
        TimeRoss) 
endif()
