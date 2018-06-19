// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#pragma once

#include "UnitHolder.h"

class CGraphanDicts;

class CDictionary;

class CGraphmatFile : public CUnitHolder {
    string m_SourceFileName;
    string m_LastError;


    // Analyzing
    bool IsKey(size_t LB, size_t HB, size_t &GraLast) const;

    bool FindKeySequence(const char *title, size_t i, size_t HB, size_t &GraLast) const;

    bool DealBullet(size_t i, size_t HB);

    bool DealAsteriskBullet(size_t LB, size_t HB);

    int DealBulletsWithTwoBrackets(size_t StartPos, size_t EndPos);

    bool DealEnglishStyleFIO(size_t StartPos, size_t EndPos);

    bool DealAbbrev(size_t StartPos, size_t EndPos);

    size_t FindOborotto(size_t i, size_t HB, short &OborotNo, vector<WORD> &OborortIds) const;

    void DealOborotto(size_t HB);

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

    int DealShortFIO(size_t i, size_t HB);

    void DealNames(size_t LB, size_t HB);

    bool DealSentBreaker();

    void InitNonContextDescriptors(CGraLine &L);

    bool GraphmatMain();

    int InitContextDescriptors(size_t LB, size_t HB);

    void MacSynHierarchy();


public:
    const CGraphanDicts *m_pDicts;
    bool m_bConvertRussianJo2Je;

    size_t m_MinParOfs;
    size_t m_MaxParOfs;
    size_t m_TabSize;
    // ������������ �����  ����� � ����� ������� �����������
    size_t m_MaxUpLen;
    // file for graphematical table
    string m_GraOutputFile;
    // file for macro-syntax
    string m_XmlMacSynOutputFile;
    // ����� �� ��������� ������� �� �����������.
    bool m_bSentBreaker;
    // ������ � ������� ��������
    bool m_bMacSynHierarchy;
    // � ����� �� ����� ��������� ��, ������� �� ����� ���������  �� ��������?
    // ���� m_bSubdueWrongMacroSynUnitToMainRoot = true, �� ��������� ��������� ����� �� ����� ������������������� ������.
    // ���� m_bSubdueWrongMacroSynUnitToMainRoot = false, �� ��������� ��������� ����� �� ����� � ���������� �� ��������� ����������
    bool m_bSubdueWrongMacroSynUnitToMainRoot;
    // �������, ��� ����������� ������ ���������
    bool m_bSentenceAsParagraph;
    // ����� �� �������������� �������� � ��������� ������ �������� �����, ���� ��� ��������
    // (����������� ������)
    bool m_bForceToRus;

    // ������ ������ �������� ������������ ������� � �����������
    bool m_bEmptyLineIsSentenceDelim;

    // ������������ ��� </p> ��� ���������� �� ���������
    bool m_bUseParagraphTagToDivide;

    // ������������ ������� ������ ��� ���������� �� ���������
    bool m_bUseIndention;

    bool m_bFilterUnprintableSymbols;

    bool m_bRecognizeShortFIOs;
    // max length of sentence
    size_t m_MaxSentenceLength;


    CGraphmatFile();

    ~CGraphmatFile();

    // processing
    bool LoadDicts();

    bool LoadStringToGraphan(const string &szBuffer);

    bool LoadFileToGraphan(const string &CommandLine);

    void FreeDicts();

    const string &GetLastError() const;

    //  browsing results

    void GetGraphematicalLine(char *line, size_t NumLine) const;

    MorphLanguageEnum GetTokenLanguage(int LineNo) const;

    bool StartsFixedOborot(size_t LineNo) const;

    const CDictionary *GetOborDic() const;

    void WriteGraphMat(const char *FName) const;
};

