#pragma once

#include "SemanticRusStructure.h"
#include "seman/Transfer/EngSemStructure.h"

class CSemStructureBuilder {
public:
    CEngSemStructure m_EngStr;
    CRusSemStructure m_RusStr;
    long m_CurrentSentence;
    CMyTimeSpanHolder m_GlobalSpan;
    bool m_bShouldBuildTclGraph;
    std::vector<CRusSemStructure> m_SavedSentences;

    CSemStructureBuilder(void);

    ~CSemStructureBuilder(void);

    void InitDicts();

    bool FindSituationsForNextSentence();

    void FindSituations(std::string text, CSemOptions options=CSemOptions());

    void TranslateToEnglish(CSemOptions options = CSemOptions());

    bool BuildSentence(std::string &Sentence);

    bool SyntRusSentence(std::string &Sentence);

    void SaveThisSentence(void);

    void ClearSavedSentences(void);

    long GetScrollMax() const;

    long GetScrollCurrent() const;

    std::string TranslateRussianText(const std::string &russian, const std::string &po);

};

extern void init_plog_seman(plog::Severity severity, std::string filename);
