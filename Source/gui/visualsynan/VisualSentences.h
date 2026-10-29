#pragma once

// the visual representation of synan results: words, clauses and groups with their coordinates
// (ported from windows/VisualSynan: VisualSentences.h, SentenceLine.h)

#include "synan/SynanLib/SentencesCollection.h"

#include <QFont>
#include <QFontMetrics>
#include <QPoint>
#include <QRgb>
#include <QString>

#include <map>
#include <vector>

class QPainter;
class CSyntaxHolder;

#define RADIUS_ANGLE 8
#define SPACE_BETWEEN_SENTENCE 20

// a rectangle with the semantics of MFC CRect (right and bottom are exclusive)
struct Rect {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    Rect() = default;
    Rect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
    void set(int l, int t, int r, int b) { left = l; top = t; right = r; bottom = b; }
    void setEmpty() { set(0, 0, 0, 0); }
    bool isEmpty() const { return right <= left || bottom <= top; }
    bool contains(const QPoint& p) const { return p.x() >= left && p.x() < right && p.y() >= top && p.y() < bottom; }
    int width() const { return right - left; }
};

// the fonts of the view, the words font is the default one
struct VisualFonts {
    QFont m_Words;
    QFont m_Bold;
    QFont m_Underline;
    QFont m_BoldUnderline;
    QFont m_GroupNames;

    void setWordsFont(const QFont& f);
};

extern VisualFonts g_VisualFonts;

struct SFigure {
    SFigure(int iNum, int iPartNum) : m_iNum(iNum), m_iPartNum(iPartNum) {}
    int m_iNum;
    int m_iPartNum;
};

typedef std::vector<SFigure> CFigureVector;

class CSentenceLine {
public:
    void Reset() {
        m_vectorWordsNum.clear();
        m_vectorClauseArc.clear();
    }

    // numbers of words on this line
    std::vector<int> m_vectorWordsNum;
    // numbers and parts of arcs on this line
    CFigureVector m_vectorClauseArc;
    Rect m_rectLine;
};

enum ETypeFigure { ArcFig, Line };

struct CPartOfGrArc {
    bool m_bHasDescription = false;
    int m_iLine = -1;
    bool m_bWholeArc = false;
    QRgb m_Color = 0;
    QPoint m_pointStart;
    QPoint m_pointEnd;
    Rect m_RectForArc;
    ETypeFigure type = Line;
};

class CVisualWord;
typedef std::vector<CVisualWord*> CWordPtrVector;

struct CVisualPeriod {
    int m_iSpaceBetweenLines = 70;
    int m_iFirstWord = -1;
    int m_iLastWord = -1;
    int m_iLevel = -1;
    QString m_strDescription;
    std::vector<CPartOfGrArc> m_vectorParts;

    void SetSpaceBetweenLines(int iVal) { m_iSpaceBetweenLines = iVal; }
};

struct CVisualGroup : public CVisualPeriod {
    QPoint m_pointLeft;
    QPoint m_pointRight;
    bool m_bClause = false;

    CVisualGroup();
    void Init(const CClause& clause, const CGroup& piGroup);
    void CalculateCoordinates(const QPoint& pointLeftLeg, const QPoint& pointRightLeg, int top, int iWidth, bool bOnDifferentLines, QRgb Color, int iLine);
    void PrintGroupPart(QPainter& p, int i) const;
};

class CVisualGroups {
public:
    void CalculateGroupsCoordinates(const QFontMetrics& fm, int iWidth, int& iLine, int iPrevBottom);
    void SetWordsArray(CWordPtrVector* pArr) { m_pWordsArr = pArr; }

protected:
    bool GetRightLegPointForGroupArc(int iGroupNum, QPoint* pPoint, int iWidth);
    bool GetLeftLegPointForGroupArc(int iGroupNum, QPoint* pPoint, int iWidth);
    std::vector<CVisualGroup*> m_arrActiveGroups;
    CWordPtrVector* m_pWordsArr = nullptr;
    int m_iMaxGroupLevel = 0;
    int m_iSpaceBetweenLinesG = 70;
};

struct SUnit {
    EUnitType m_Type;
    int m_iClauseNum = -1;
    int m_iWordNum = -1;
    int m_iHommonyNum = -1;
    int m_iClauseTypeNum = -1;
    QString m_strOborotsNum;
    QString m_strGrammems;
};

class CVisualSynVariant {
public:
    std::vector<SUnit> m_vectorUnits;
    std::vector<CVisualGroup> m_Groups;
    std::vector<int> m_Subjects;
    int m_iPredk = -1;
    int m_iClauseTypeNum = -1;
    int m_Weight = 0;

    void InitClauseVariant(const CClause& piClause, int ClauseVariantNo);
    bool operator==(const std::vector<SUnit>& vectorUnits) const;
};

class CClauseType {
public:
    bool Init(const CClause& clause, const SClauseType& piClauseType);
    bool HasHom(int iHom) const;

    std::vector<int> m_arrHomNum;
    int m_iWordNum = 0;
    bool m_bHomChoosen = false;
    QString m_strName;
};

class CVisualClause : public CVisualGroup, public CVisualGroups {
    friend class CVisualSentence;

public:
    bool InitVisualClause(const CClause& piClause);
    bool FillActiveGroupsArray(std::map<int, int>& mapClauseNumTypeNum, int iClauseNum);
    void SetBestHomonyms();

protected:
    void SetActiveType(int iType);
    QString m_strConjsName;
    std::vector<CVisualSynVariant> m_SynVars;
    std::vector<CClauseType> m_arrTypes;
    int m_iActiveType = -1;
    bool m_bBestVariant = true;
    QString m_strTypeName;
    int m_Weight = 0;
};

struct CVisualHomonym {
    bool m_bPredk = false;
    bool m_bSubj = false;
    QString m_strLemma;
    QString m_strCommonGrammems;
    QString m_strPOS;
    QString m_strSomeDescr;
    QString m_strOborotsNum;
    uint32_t m_iPradigmID = 0;
};

class CVisualWord {
public:
    bool Init(const CSynWord& piWord, const CSentence& piSentence);
    int GetWordLen(const QFontMetrics& fm) const;
    int CalculateCoordinates(const QFontMetrics& fm, int iX, int iY);
    void PrintWord(QPainter& p) const;
    bool PointInWordRect(const QPoint& point) const { return m_WordRect.contains(point); }
    bool SetActiveHomonym(int iActiveHomonim);
    QString GetActiveHomDescr() const;
    void ResetSubjAndPred();
    const CVisualHomonym& ActiveHomonym() const { return m_arrHomonyms[m_iActiveHomonym]; }
    CVisualHomonym& ActiveHomonym() { return m_arrHomonyms[m_iActiveHomonym]; }

    QString m_strWord;
    QString m_strSomeDescr;
    Rect m_WordRect;
    int m_iActiveHomonym = 0;
    std::vector<CVisualHomonym> m_arrHomonyms;
    bool m_bInTermin = false;
    bool m_bArtificialCreated = false;
    QString m_strActiveGrammems;
    int m_iClauseNo = -1;
    int m_ReferenceWordNo = -1;
    std::vector<int> m_MainVerbs;

protected:
    bool m_bBold = false;
};

class CVisualSentence : public CVisualGroups {
public:
    ~CVisualSentence();
    bool Init(const CSentence& piSentence);
    int CalculateCoordinates(const QFontMetrics& fm, int iY, int iWidth);
    void PrintAllLines(QPainter& p) const;
    bool PointInSentRect(const QPoint& point) const { return m_SentRect.contains(point); }
    CVisualWord* GetWordByPoint(const QPoint& point, int* iActiveWord);
    bool SetActiveHomonym(int iActiveWord, int iActiveHomonim);
    QString GetActiveHomDescr(int iActiveWord) const;

    Rect m_SentRect;

protected:
    int CalculateSpaceBetweenLines(const QFontMetrics& fm);
    void FillActiveGroupsArray();
    void FillLinesVector();
    void FillClauseLine(CFigureVector& vectorFigure, int lineTop, int lineBottom);
    void PrintLine(QPainter& p, int iLine) const;
    bool ReadWords(const CSentence& piSentence);
    bool ReadClauses(const CSentence& piSentence);

    std::vector<CVisualClause*> m_arrClauses;
    CWordPtrVector m_arrWords;
    int m_iMaxClausesLevel = 0;
    std::vector<CSentenceLine> m_vectorSentLines;
};

class CVisualSentences {
public:
    ~CVisualSentences() { Reset(); }
    void Reset();
    int SentCount() const { return (int)m_arrSentences.size(); }
    bool FillSentencesArray(const CSentencesCollection& coll);
    void CalculateCoordinates(const QFontMetrics& fm, int iWidth);
    void PrintSentences(QPainter& p) const;
    // returns the word under the point
    CVisualWord* GetWordByPoint(const QPoint& point, int* iActiveSentence, int* iActiveWord);
    bool SetActiveHomonym(int iActiveSentence, int iActiveWord, int iActiveHomonim);
    QString GetActiveHomDescr(int iActiveSentence, int iActiveWord) const;
    QSize GetSentencesSize() const;

private:
    std::vector<CVisualSentence*> m_arrSentences;
};

// the report "Build best rels" for the last sentences of the holder
QString BuildRels(const CSentencesCollection& coll);

QString FromRMLEncode(const std::string& s);
