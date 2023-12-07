// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#pragma once

#include "UnitHolder.h"

class CGraphanDicts;

class CDictionary;

class CGraphmatFile : public CUnitHolder {
    std::string m_LastError;


    // Analyzing
    bool IsKey(size_t LB, size_t HB, size_t &GraLast) const;

    bool FindKeySequence(const char *title, size_t i, size_t HB, size_t &GraLast) const;

    bool DealBullet(size_t i, size_t HB);

    bool DealAsteriskBullet(size_t LB, size_t HB);

    int DealBulletsWithTwoBrackets(size_t StartPos, size_t EndPos);

    bool DealEnglishStyleFIO(size_t StartPos, size_t EndPos);

    bool DealAbbrev(size_t StartPos, size_t EndPos);

    size_t FindOborotto(size_t i, size_t HB, const std::vector<uint16_t> &OborortIds);

    void DealOborotto();

    int DealReferences(size_t i, size_t HB);

    int DealStop(size_t i);

    void DealModifierKey(size_t LB, size_t HB);

    void DealSimpleKey(size_t LB, size_t HB);

    void DealKeySequence(size_t LB, size_t HB);

    void DealQuotedKeySequence(size_t LB, size_t HB);

    void DealGermanDividedCompounds(size_t LB, size_t HB);

    void DealExtensionsAndLocalFileNames(size_t LB, size_t HB);

    int HasIndention(size_t LB, size_t HB);

    int CountEndL(size_t LB, size_t HB);

    int CountSpaces(size_t LB, size_t HB);

    int DealFIO(size_t i, size_t HB);

    void DealSentBreaker();

    void GraphmatMain();

    void InitContextDescriptors();

    void MacSynHierarchy();

    void InitEnglishNameSlot();

    bool CheckSentenceStart(size_t StartSent, size_t EndPos) const;

    int FindSentenceStart(size_t WordNo, size_t EndPos) const;

    void ReadToken(const char* in_str, CGraLine& token) const;

    size_t ReadWord(const char* s, CGraLine& token) const;

    void ReadIdents(CGraphanDicts* dicts, std::string path);

    void DealIdents();

    bool DealSimpleEnglishNames(size_t StartPos);

public:
    const CGraphanDicts *m_pDicts;

    size_t m_MinParOfs;
    size_t m_MaxParOfs;
    size_t m_TabSize;
    // максимальное число  строк в одном условно предложении
    size_t m_MaxUpLen;
    // file for macro-syntax
    std::string m_XmlMacSynOutputFile;
    // Нужно ли запускать делилку на предложения.
    bool m_bSentBreaker;
    // нужжно и строить иерархию
    bool m_bMacSynHierarchy;
    // С каким УП нужно связывать УП, которые не могут связаться  по правилам?
    // Если m_bSubdueWrongMacroSynUnitToMainRoot = true, то программа подчиняет такие УП корню макросинтаксичесого дерева.
    // Если m_bSubdueWrongMacroSynUnitToMainRoot = false, то программа связывает такие УП корню с предыдущим УП отношение примыкания
    bool m_bSubdueWrongMacroSynUnitToMainRoot;
    // считать, что предложение рапвно параграфу
    bool m_bSentenceAsParagraph;
    // нужно ли конвертировать латиницу в кириллицу внутри русского слова, если это возможно
    // (исправление ошибок)
    bool m_bForceToRus;

    // пустая строка является разделителем абазцев и предложений
    bool m_bEmptyLineIsSentenceDelim;

    // использовать таг </p> для разделения на параграфы
    bool m_bUseParagraphTagToDivide;

    // использовать красную строку для разделения на параграфы
    bool m_bUseIndention;

    // max length of sentence
    size_t m_MaxSentenceLength;


    CGraphmatFile();

    ~CGraphmatFile();

    // processing
    bool LoadDicts(MorphLanguageEnum langua);

    void LoadStringToGraphan(const std::string &szBuffer);

    void LoadFileToGraphan(const std::string& path);

    void FreeDicts();

    const std::string &GetLastError() const;

    const CDictionary *GetOborDic() const;

    void WriteGraphMat(std::string FName) const;

};

