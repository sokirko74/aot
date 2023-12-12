declare_cmake_min_version()

project(SemanLib)

add_library(SemanLib
        AllRossesHolder.cpp
        DictUnitInterp.cpp
        GenerateNewForm.cpp
        HierarchyHolder.cpp
        "struct_dict_holder.cpp"
        RussianSynthesis.cpp
        SemPattern.cpp
        SemStructureBuilder.cpp
        SemanHypotGraph.cpp
        SemanPattern.cpp
        "answers.cpp"
        SemanticClause.cpp
        SemanticColloc.cpp
        SemanticCommon.cpp
        SemanticCompar.cpp
        SemanticGraph.cpp
        SemanticIdealize.cpp
        SemanticLength.cpp
        SemanticLexFuncts.cpp
        "lex_variant.cpp"
        SemanticLoc.cpp
        SemanticMNA.cpp
        SemanticNumeral.cpp
        SemanticRusPrimitives.cpp
        SemanticStrAn.cpp
        SemanticSynRelation.cpp
        SemanticThesaurus.cpp
        "time_phrases.cpp"
        SemanticTree.cpp
        SemanticWeight.cpp
        SemanticsHolder.cpp
        TransferSyntaxRelationToSemantics.cpp
        TranslatorHolder.cpp
        Valency.cpp
        stdafx.cpp
        VisualGraph.cpp
        SemanticWeightComponents.cpp
        AllRossesHolder.h
        DictUnitInterp.h
        HierarchyHolder.h
        LexFuncts.h
        "struct_dict_holder.h"
        SemanticRusStructure.h
        SemanticsHolder.h
        SemanticStructure.h
        SemanticWeight.h
        SemPattern.h
        SemStructureBuilder.h
        stdafx.h
        TranslatorHolder.h
        Valency.h
        VisualGraph.h
        "analytical.cpp" 
        "fem_deriv.cpp" 
        "rus_sem_word.cpp" 
        "rus_sem_word.h"
        "sem_word.cpp" 
        "sem_word.h"
        "dict_type.h" 
        "dict_type.cpp" 
        "rus_sem_relation.cpp" "rus_sem_relation.h" "sem_relation.cpp" "sem_relation.h" 
        "syn_realization.h" "syn_realization.cpp" "time_phrases.h")

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

add_subdirectory(tests)
