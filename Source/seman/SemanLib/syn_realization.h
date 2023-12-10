#pragma once 

struct CSynRealization {
    // предлоги
    std::vector<CRossInterp> m_Preps;
    // союзы (для отношений)
    CRossInterp m_Conj;

    // граммемы отношения или узла
    grammems_mask_t m_Grammems;

    // предлог или союз, который не был найден в оборотах, но он был прописан в статье.
    //  Такой предлог или союз пишется прямо в строку.
    std::string m_Other;

    // номер используемого кортежа в векторе CSemPattern::m_GramCorteges
    long m_CortegeNo;

    // сохраненный кортеж  CSemPattern::m_GramCorteges[m_CortegeNo]
    TCortege m_Cortege;

    // добавлено для статьи "не ранее", в которой используется специальная константа А1(НЕ)
    std::string m_AlgStr;

    void SetEmpty();

    // проверяет, приписан ли узлу предлог PrepNo
    bool HasThePrep(uint16_t UnitNo) const;

};
