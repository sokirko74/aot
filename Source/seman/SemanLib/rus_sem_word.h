#pragma once

#include "sem_word.h"

class CRusSemWord : public CSemWord {
    void Init();
    // входное слово прописными буквами
    std::string m_WordUpper;

public:
    // графематическая помета ЗПР
    bool m_IsPunct;
    // графематическая помета ИЛЕ
    bool m_ILE;
    // знаки препинаия, которые стоят в тексте после этого слова
    std::string m_PostPuncts;
    std::string m_GraphDescrs;
    //  лемма прилагательного превосходной степени, которая была заменена на положительную степень
    std::string m_SupAdjLemma;
    bool m_bRomanNumber;
    long m_SynWordNo;

    CRusSemWord();

    CRusSemWord(long WordNo, std::string Lemma);

    // словоформа - глагольная форма, которая заканчивается на "ся", "сь"
    bool IsReflexiveVerb() const;

    uint32_t GetRusRichPoses() const;

    bool HasPOS(part_of_speech_t POS) const;

    bool IsEqualMorph(const CRusSemWord& W) const;

    void InitWordFeatures(const CSentence* piSent, long WordNo);

    const std::string& GetWord() const override;

    void SetWord(const std::string& s);

    // является ли данное слово кавычкой
    bool IsQuoteMark() const;

};
