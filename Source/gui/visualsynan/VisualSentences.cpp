// ported from windows/VisualSynan: VisualSentences.cpp, VisualSentence.cpp, VisualClause.cpp,
// VisualGroup.cpp, VisualGroups.cpp, VisualWord.cpp

#include "VisualSentences.h"

#include "synan/SynCommonLib/RelationsIterator.h"

#include <QPainter>

#include <algorithm>
#include <cmath>

VisualFonts g_VisualFonts;

void VisualFonts::setWordsFont(const QFont& f) {
    m_Words = f;
    m_Bold = f;
    m_Bold.setBold(true);
    m_Underline = f;
    m_Underline.setUnderline(true);
    m_BoldUnderline = m_Bold;
    m_BoldUnderline.setUnderline(true);
    m_GroupNames = f;
    m_GroupNames.setPixelSize(std::max(1, (f.pixelSize() / 3) * 2));
}

QString FromRMLEncode(const std::string& s) {
    return QString::fromUtf8(s.c_str());
}

// TextOut of MFC: y is the top of the text
static void textOut(QPainter& p, int x, int yTop, const QString& s) {
    p.drawText(x, yTop + p.fontMetrics().ascent(), s);
}

// Arc of GDI: a counterclockwise arc of the ellipse inscribed in the rectangle from the
// radial line through start to the radial line through end; start == end gives the whole ellipse
static void gdiArc(QPainter& p, const Rect& r, const QPoint& start, const QPoint& end) {
    QRectF rect = QRectF(QPointF(r.left, r.top), QPointF(r.right, r.bottom)).normalized();
    if (rect.width() <= 0 || rect.height() <= 0) {
        return;
    }
    QPointF c = rect.center();
    double a1 = std::atan2(-(start.y() - c.y()), start.x() - c.x()) * 180 / M_PI;
    double a2 = std::atan2(-(end.y() - c.y()), end.x() - c.x()) * 180 / M_PI;
    double span = a2 - a1;
    while (span <= 0) {
        span += 360;
    }
    p.drawArc(rect, (int)std::lround(a1 * 16), (int)std::lround(span * 16));
}

static CPartOfGrArc NewGroupArcPart(QRgb color, int line, QPoint start, QPoint end, ETypeFigure type, Rect rectForArc, bool has_descr, bool whole_arc) {
    CPartOfGrArc part;
    part.m_Color = color;
    part.m_iLine = line;
    part.m_pointStart = start;
    part.type = type;
    part.m_pointEnd = end;
    part.m_RectForArc = rectForArc;
    part.m_bHasDescription = has_descr;
    part.m_bWholeArc = whole_arc;
    return part;
}

static void normalizePeriod(const CVisualPeriod* p, int& first, int& last) {
    if (p->m_iFirstWord < p->m_iLastWord) {
        first = p->m_iFirstWord;
        last = p->m_iLastWord;
    }
    else {
        last = p->m_iFirstWord;
        first = p->m_iLastWord;
    }
}

static bool PeriodsCompare_sort(const CVisualPeriod* pPeriod1, const CVisualPeriod* pPeriod2) {
    int iFirstWord1, iLastWord1, iFirstWord2, iLastWord2;
    normalizePeriod(pPeriod1, iFirstWord1, iLastWord1);
    normalizePeriod(pPeriod2, iFirstWord2, iLastWord2);
    if (iFirstWord1 == iFirstWord2 && iLastWord1 == iLastWord2) {
        return std::less<const CVisualPeriod*>()(pPeriod1, pPeriod2);
    }
    if (iFirstWord1 >= iFirstWord2 && iLastWord1 <= iLastWord2) {
        return true;
    }
    if (iLastWord1 <= iFirstWord2) {
        return true;
    }
    return false;
}

static int AssignPeriodLevel(std::vector<CVisualGroup*>& pPeriodArr) {
    int iMaxGroupLevel = 1;
    for (size_t i = 0; i < pPeriodArr.size(); i++) {
        CVisualPeriod* pPeriod = pPeriodArr[i];
        int iFirstWord, iLastWord;
        normalizePeriod(pPeriod, iFirstWord, iLastWord);
        int iMaxLevel = 0;
        for (int j = (int)i - 1; j >= 0; j--) {
            CVisualPeriod* pPeriod1 = pPeriodArr[j];
            int iFirstWord1, iLastWord1;
            normalizePeriod(pPeriod1, iFirstWord1, iLastWord1);
            if (!(iLastWord1 <= iLastWord && iFirstWord1 >= iFirstWord)) {
                break;
            }
            if (iMaxLevel < pPeriod1->m_iLevel) {
                iMaxLevel = pPeriod1->m_iLevel;
            }
        }
        if (iMaxLevel + 1 > iMaxGroupLevel) {
            iMaxGroupLevel = iMaxLevel + 1;
        }
        pPeriod->m_iLevel = iMaxLevel + 1;
    }
    return iMaxGroupLevel;
}

//----------------------------------------------------------------------------
// CVisualGroup

CVisualGroup::CVisualGroup() {
    m_iFirstWord = 0;
    m_iLastWord = 0;
    m_iLevel = 0;
}

void CVisualGroup::Init(const CClause& clause, const CGroup& piGroup) {
    m_iFirstWord = piGroup.m_iFirstWord;
    m_iLastWord = piGroup.m_iLastWord;
    m_strDescription = FromRMLEncode(clause.GetOpt()->GetGroupNameByIndex(piGroup.m_GroupType)).toLower();
}

void CVisualGroup::CalculateCoordinates(const QPoint& pointLeftLeg, const QPoint& poitRightLeg, int top, int iWidth, bool bOnDifferentLines, QRgb Color, int iLine) {
    m_vectorParts.clear();
    m_vectorParts.push_back(NewGroupArcPart(Color, iLine, pointLeftLeg, QPoint(pointLeftLeg.x(), top + RADIUS_ANGLE),
        Line, Rect(), false, false));
    m_vectorParts.push_back(NewGroupArcPart(Color, iLine, QPoint(pointLeftLeg.x() + RADIUS_ANGLE, top), QPoint(pointLeftLeg.x(), top + RADIUS_ANGLE + 2),
        ArcFig, Rect(pointLeftLeg.x(), top, pointLeftLeg.x() + 2 * RADIUS_ANGLE - 2, top + 2 * RADIUS_ANGLE), false, false));

    QPoint _Point;
    // if groups are on different lines
    if (bOnDifferentLines) {
        m_vectorParts.push_back(NewGroupArcPart(Color, iLine, QPoint(pointLeftLeg.x() + RADIUS_ANGLE, top), QPoint(iWidth, top),
            Line, Rect(), false, false));
        iLine++;
        int k = 1;
        int step = m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE;
        while (poitRightLeg.y() - top > (k + 1) * step) {
            m_vectorParts.push_back(NewGroupArcPart(Color, iLine, QPoint(0, top + k * step), QPoint(iWidth, top + k * step),
                Line, Rect(), true, false));
            k++;
        }
        m_vectorParts.push_back(NewGroupArcPart(Color, iLine, QPoint(0, top + k * step), QPoint(poitRightLeg.x() - RADIUS_ANGLE + 1, top + k * step),
            Line, Rect(), true, false));
        m_vectorParts.push_back(NewGroupArcPart(Color, iLine, QPoint(poitRightLeg.x(), top + k * step + RADIUS_ANGLE), QPoint(poitRightLeg.x() - RADIUS_ANGLE + 1, top + k * step - 1),
            ArcFig, Rect(poitRightLeg.x() - 2 * RADIUS_ANGLE, top + k * step, poitRightLeg.x() + 1, top + 2 * RADIUS_ANGLE + k * step), false, false));
        _Point = QPoint(poitRightLeg.x(), top + k * step + RADIUS_ANGLE);
    }
    else {
        m_vectorParts.push_back(NewGroupArcPart(Color, iLine, QPoint(pointLeftLeg.x() + RADIUS_ANGLE, top), QPoint(poitRightLeg.x() - RADIUS_ANGLE + 1, top),
            Line, Rect(), true, true));
        m_vectorParts.push_back(NewGroupArcPart(Color, iLine, QPoint(poitRightLeg.x() + 1, top + RADIUS_ANGLE), QPoint(poitRightLeg.x() - RADIUS_ANGLE + 1, top),
            ArcFig, Rect(poitRightLeg.x() - 2 * RADIUS_ANGLE + 3, top, poitRightLeg.x() + 1, top + 2 * RADIUS_ANGLE), false, true));
        _Point = QPoint(poitRightLeg.x(), top + RADIUS_ANGLE);
    }
    m_vectorParts.push_back(NewGroupArcPart(Color, iLine, _Point, poitRightLeg, Line, Rect(), false, false));

    m_pointLeft = QPoint(pointLeftLeg.x(), top);
    m_pointRight.setX(poitRightLeg.x());
    if (bOnDifferentLines) {
        m_pointRight.setY(top + m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE);
    }
    else {
        m_pointRight.setY(top);
    }
}

void CVisualGroup::PrintGroupPart(QPainter& p, int i) const {
    if (i < 0 || i >= (int)m_vectorParts.size()) {
        return;
    }
    const CPartOfGrArc& part = m_vectorParts[i];
    p.save();
    p.setPen(QPen(QColor(part.m_Color), 1));
    if (part.type == ArcFig) {
        gdiArc(p, part.m_RectForArc, part.m_pointStart, part.m_pointEnd);
    }
    else {
        p.drawLine(part.m_pointStart, part.m_pointEnd);
    }
    p.restore();

    if (part.m_bHasDescription) {
        p.save();
        p.setFont(g_VisualFonts.m_GroupNames);
        int height = p.fontMetrics().height();
        if (!part.m_bWholeArc) {
            textOut(p, 5, part.m_pointStart.y() - height - 2, m_strDescription);
        }
        else {
            textOut(p, m_pointLeft.x() + (m_pointRight.x() - m_pointLeft.x()) / 2 + 2, part.m_pointStart.y() - height - 2, m_strDescription);
        }
        p.restore();
    }
}

//----------------------------------------------------------------------------
// CVisualGroups

void CVisualGroups::CalculateGroupsCoordinates(const QFontMetrics& fm, int iWidth, int& iLine, int iPrevBottom) {
    int tmHeight = fm.height();
    for (size_t i = 0; i < m_arrActiveGroups.size(); i++) {
        CVisualGroup* pGroup = m_arrActiveGroups[i];
        QRgb Color = pGroup->m_bClause ? qRgb(255, 0, 0) : qRgb(0, 0, 255);

        const Rect& rectFirstWord = (*m_pWordsArr)[pGroup->m_iFirstWord]->m_WordRect;
        const Rect& rectLastWord = (*m_pWordsArr)[pGroup->m_iLastWord]->m_WordRect;
        int iBottom = std::min(rectFirstWord.bottom, rectLastWord.bottom);
        bool bOnDifferentLines = (rectFirstWord.bottom != rectLastWord.bottom);
        if (pGroup->m_iLevel == 0) {
            return;
        }
        int space = m_iSpaceBetweenLinesG - (tmHeight + (tmHeight / 3) * 2);
        int yCoef = (space / m_iMaxGroupLevel) * (m_iMaxGroupLevel - pGroup->m_iLevel);

        QPoint pointLeftLeg;
        if (!GetLeftLegPointForGroupArc((int)i, &pointLeftLeg, iWidth)) {
            return;
        }
        QPoint pointRightLeg;
        if (!GetRightLegPointForGroupArc((int)i, &pointRightLeg, iWidth)) {
            return;
        }
        int top = rectFirstWord.top - space + yCoef;
        if (bOnDifferentLines) {
            bOnDifferentLines = !(pointLeftLeg.y() < rectFirstWord.bottom && pointRightLeg.y() < rectFirstWord.bottom);
        }
        if (iBottom > iPrevBottom && !bOnDifferentLines) {
            iLine++;
        }
        pGroup->SetSpaceBetweenLines(m_iSpaceBetweenLinesG);
        pGroup->CalculateCoordinates(pointLeftLeg, pointRightLeg, top, iWidth, bOnDifferentLines, Color, iLine);
        iPrevBottom = iBottom;
    }
}

bool CVisualGroups::GetLeftLegPointForGroupArc(int iGroupNum, QPoint* pPoint, int iWidth) {
    CVisualGroup* pGroupForDrawing = m_arrActiveGroups[iGroupNum];
    bool bExistUsefulGroup = false;
    int i = iGroupNum - 1;
    for (; i >= 0; i--) {
        if (m_arrActiveGroups[i]->m_iFirstWord == pGroupForDrawing->m_iFirstWord) {
            bExistUsefulGroup = true;
            break;
        }
    }
    if (bExistUsefulGroup && !(pGroupForDrawing->m_bClause && !m_arrActiveGroups[i]->m_bClause)) {
        CVisualGroup* pGroup = m_arrActiveGroups[i];
        if (pGroup->m_pointLeft.x() == 0 && pGroup->m_pointRight.x()) {
            return false;
        }
        pPoint->setY(pGroup->m_pointLeft.y());
        pPoint->setX(pGroup->m_pointLeft.x() + 10);
    }
    else {
        CVisualWord* pWord = (*m_pWordsArr)[pGroupForDrawing->m_iFirstWord];
        if (pWord->m_WordRect.isEmpty()) {
            return false;
        }
        pPoint->setX(pWord->m_WordRect.left + 10);
        pPoint->setY(pWord->m_WordRect.top);
    }
    return true;
}

bool CVisualGroups::GetRightLegPointForGroupArc(int iGroupNum, QPoint* pPoint, int iWidth) {
    CVisualGroup* pGroupForDrawing = m_arrActiveGroups[iGroupNum];
    CVisualGroup* pGroup = nullptr;
    bool bExistUsefulGroup = false;
    if (iGroupNum > 0) {
        pGroup = m_arrActiveGroups[iGroupNum - 1];
        if (pGroup->m_iLastWord == pGroupForDrawing->m_iLastWord) {
            bExistUsefulGroup = true;
        }
    }
    if (bExistUsefulGroup && !(pGroupForDrawing->m_bClause && !pGroup->m_bClause)) {
        pPoint->setY(pGroup->m_pointLeft.y());
        if (pGroup->m_pointLeft.y() == pGroup->m_pointRight.y()) {
            pPoint->setX(pGroup->m_pointRight.x() - 10);
        }
        else {
            // if this group is on different lines
            pPoint->setX(pGroup->m_pointRight.x() + iWidth - 10);
        }
    }
    else {
        CVisualWord* pWord = (*m_pWordsArr)[pGroupForDrawing->m_iLastWord];
        if (pWord->m_WordRect.isEmpty()) {
            return false;
        }
        pPoint->setX(pWord->m_WordRect.right - 10);
        pPoint->setY(pWord->m_WordRect.top);
    }
    return true;
}

//----------------------------------------------------------------------------
// CVisualSynVariant, CClauseType

bool CVisualSynVariant::operator==(const std::vector<SUnit>& vectorUnits) const {
    if (m_vectorUnits.size() != vectorUnits.size()) {
        return false;
    }
    for (size_t i = 0; i < m_vectorUnits.size(); i++) {
        if (vectorUnits[i].m_Type != m_vectorUnits[i].m_Type
            || vectorUnits[i].m_iClauseNum != m_vectorUnits[i].m_iClauseNum
            || vectorUnits[i].m_iWordNum != m_vectorUnits[i].m_iWordNum
            || vectorUnits[i].m_iHommonyNum != m_vectorUnits[i].m_iHommonyNum
            || vectorUnits[i].m_iClauseTypeNum != m_vectorUnits[i].m_iClauseTypeNum) {
            return false;
        }
    }
    return true;
}

void CVisualSynVariant::InitClauseVariant(const CClause& piClause, int ClauseVariantNo) {
    CSVI pSynVar = piClause.GetSynVariantByNo(ClauseVariantNo);
    if (!piClause.IsValidCSVI(pSynVar)) {
        return;
    }
    for (const auto& in_unit : pSynVar->m_SynUnits) {
        SUnit unit;
        unit.m_Type = in_unit.m_Type;
        if (unit.m_Type == EClause) {
            int iClauseNum = piClause.m_pSent->FindClauseIndexByPeriod(in_unit.m_SentPeriod);
            if (iClauseNum == -1) {
                continue;
            }
            unit.m_iClauseNum = iClauseNum;
            unit.m_iClauseTypeNum = in_unit.m_iClauseTypeNum;
        }
        else {
            for (const auto& prep_no : in_unit.m_SimplePrepNos) {
                unit.m_strOborotsNum += "Ob: " + FromRMLEncode(piClause.GetOpt()->GetOborDic()->m_Entries[prep_no].m_OborotEntryStr);
            }
            std::string s = in_unit.GetGrammemsByAncodes();
            if (in_unit.m_Type == EWord) {
                auto& H = piClause.GetWords()[in_unit.m_SentPeriod.m_iFirstWord].m_Homonyms[in_unit.m_iHomonymNum];
                if (H.m_bPassive) {
                    s = "PASSIVE " + s;
                }
                if (in_unit.m_bReflexive) {
                    s = "REFLEXIVE " + s;
                }
            }
            unit.m_strGrammems = FromRMLEncode(s);
            unit.m_iHommonyNum = in_unit.m_iHomonymNum;
            unit.m_iWordNum = in_unit.m_SentPeriod.m_iFirstWord;
        }
        m_vectorUnits.push_back(unit);
    }
    m_iClauseTypeNum = pSynVar->m_ClauseTypeNo;
    for (const CGroup& g : pSynVar->m_vectorGroups.GetGroups()) {
        CVisualGroup Group;
        Group.Init(piClause, g);
        m_Groups.push_back(Group);
    }
    if (pSynVar->m_iPredk != -1) {
        // convert predicate to sentence coordinates
        m_iPredk = pSynVar->GetSentenceCoordinates(pSynVar->m_iPredk).m_iFirstWord;
    }
    // convert subjects to sentence coordinates
    m_Subjects.clear();
    for (auto subj : pSynVar->m_Subjects) {
        m_Subjects.push_back(pSynVar->GetSentenceCoordinates(subj).m_iFirstWord);
    }
    m_Weight = pSynVar->m_iWeight;
}

bool CClauseType::Init(const CClause& clause, const SClauseType& piClauseType) {
    try {
        m_arrHomNum.push_back(piClauseType.m_Root.m_HomonymNo);
        m_iWordNum = piClauseType.m_Root.m_WordNo;
        std::string str;
        if (piClauseType.m_Type == UnknownSyntaxElement) {
            str = "EMPTY";
        }
        else {
            str = clause.GetOpt()->GetGramTab()->GetClauseNameByType((long)piClauseType.m_Type);
        }
        m_strName = FromRMLEncode(str);
    }
    catch (...) {
        return false;
    }
    return true;
}

bool CClauseType::HasHom(int iHom) const {
    return std::find(m_arrHomNum.begin(), m_arrHomNum.end(), iHom) != m_arrHomNum.end();
}

//----------------------------------------------------------------------------
// CVisualClause

void CVisualClause::SetActiveType(int iType) {
    m_iActiveType = iType;
    if (m_iActiveType < 0 || m_iActiveType >= (int)m_arrTypes.size()) {
        m_strTypeName = "";
    }
    else {
        m_strTypeName = m_arrTypes[m_iActiveType].m_strName;
    }
    if (!m_strConjsName.isEmpty()) {
        m_strDescription = m_strTypeName + "; " + m_strConjsName;
    }
    else {
        m_strDescription = m_strTypeName;
    }
    m_strDescription += ";" + QString::number(m_Weight);
}

bool CVisualClause::InitVisualClause(const CClause& piClause) {
    try {
        m_iFirstWord = piClause.m_iFirstWord;
        m_iLastWord = piClause.m_iLastWord;
        int RelWordNo = piClause.m_RelativeWord.m_WordNo;
        if (RelWordNo != -1) {
            (*m_pWordsArr)[RelWordNo]->m_ReferenceWordNo = piClause.m_AntecedentWordNo;
        }
        m_strConjsName = FromRMLEncode(piClause.GetClauseDescr());
        bool bFirst = true;
        for (size_t i = 0; i < piClause.m_vectorTypes.size(); ++i) {
            CClauseType type;
            if (!type.Init(piClause, piClause.m_vectorTypes[i])) {
                return false;
            }
            if (bFirst && type.HasHom(0)) {
                m_iActiveType = (int)i;
                bFirst = false;
                type.m_bHomChoosen = true;
            }
            if (type.HasHom(-1) || type.HasHom(0)) {
                type.m_bHomChoosen = true;
            }
            m_arrTypes.push_back(type);
        }
        if (bFirst) {
            m_iActiveType = (int)piClause.m_vectorTypes.size() - 1;
        }
    }
    catch (...) {
        ErrorMessage("VisualSynan", "An exception occured while reading types of the clause");
        return false;
    }

    try {
        SetActiveType(m_iActiveType);
        for (size_t i = 0; i < piClause.m_SynVariants.size(); i++) {
            CVisualSynVariant SynVar;
            SynVar.InitClauseVariant(piClause, (int)i);
            m_SynVars.push_back(SynVar);
        }
        if (!m_SynVars.empty()) {
            m_Weight = m_SynVars[0].m_Weight;
        }
    }
    catch (...) {
        ErrorMessage("VisualSynan", "An exception occured while reading synvariants of the clause");
        return false;
    }
    return true;
}

void CVisualClause::SetBestHomonyms() {
    if (m_SynVars.empty()) {
        ErrorMessage("VisualSynan", "Clause with no synvariants found");
        return;
    }
    CVisualSynVariant& SynVar = m_SynVars[0];
    for (auto& unit : SynVar.m_vectorUnits) {
        if (unit.m_Type == EWord) {
            CVisualWord* pWord = (*m_pWordsArr)[unit.m_iWordNum];
            pWord->m_iActiveHomonym = unit.m_iHommonyNum;
            pWord->m_strActiveGrammems = unit.m_strGrammems;
            if (!unit.m_strOborotsNum.isEmpty()) {
                pWord->m_strSomeDescr = unit.m_strOborotsNum;
            }
            else {
                pWord->m_strSomeDescr = pWord->ActiveHomonym().m_strOborotsNum;
            }
            m_Weight = SynVar.m_Weight;
            SetActiveType(SynVar.m_iClauseTypeNum);
        }
    }
}

// chooses the syntax variant for the active homonyms of words
bool CVisualClause::FillActiveGroupsArray(std::map<int, int>& mapClauseNumTypeNum, int iClauseNum) {
    if (m_bBestVariant) {
        SetBestHomonyms();
        m_bBestVariant = false;
    }
    if (m_SynVars.empty()) {
        return false;
    }

    std::vector<SUnit> vectorUnits;
    for (auto& u : m_SynVars[0].m_vectorUnits) {
        SUnit unit;
        if (u.m_Type == EWord) {
            CVisualWord* pWord = (*m_pWordsArr)[u.m_iWordNum];
            pWord->ResetSubjAndPred();
            unit.m_iHommonyNum = pWord->m_iActiveHomonym;
            unit.m_iWordNum = u.m_iWordNum;
        }
        else {
            unit.m_iClauseTypeNum = u.m_iClauseTypeNum;
            unit.m_iClauseNum = u.m_iClauseNum;
        }
        unit.m_Type = u.m_Type;
        vectorUnits.push_back(unit);
    }

    size_t i = 0;
    for (; i < m_SynVars.size(); i++) {
        if (m_SynVars[i] == vectorUnits) {
            break;
        }
    }
    if (i == m_SynVars.size()) {
        return false;
    }

    CVisualSynVariant& SynVar = m_SynVars[i];
    m_Weight = SynVar.m_Weight;
    SetActiveType(SynVar.m_iClauseTypeNum);
    mapClauseNumTypeNum[iClauseNum] = SynVar.m_iClauseTypeNum;

    for (auto& unit : SynVar.m_vectorUnits) {
        if (unit.m_Type == EWord) {
            CVisualWord* pWord = (*m_pWordsArr)[unit.m_iWordNum];
            pWord->m_strActiveGrammems = unit.m_strGrammems;
            if (!unit.m_strOborotsNum.isEmpty()) {
                pWord->m_strSomeDescr = unit.m_strOborotsNum;
            }
            else {
                pWord->m_strSomeDescr = pWord->m_arrHomonyms[unit.m_iHommonyNum].m_strOborotsNum;
            }
        }
    }

    if (SynVar.m_iPredk != -1) {
        CVisualWord* pWord = (*m_pWordsArr)[SynVar.m_iPredk];
        pWord->ActiveHomonym().m_bPredk = true;
        for (int verb : pWord->m_MainVerbs) {
            CVisualWord* pWord1 = (*m_pWordsArr)[verb];
            pWord1->ActiveHomonym().m_bPredk = true;
            for (int verb1 : pWord1->m_MainVerbs) {
                (*m_pWordsArr)[verb1]->ActiveHomonym().m_bPredk = true;
            }
        }
    }
    for (int subj : SynVar.m_Subjects) {
        (*m_pWordsArr)[subj]->ActiveHomonym().m_bSubj = true;
    }

    m_arrActiveGroups.clear();
    for (auto& g : SynVar.m_Groups) {
        m_arrActiveGroups.push_back(&g);
    }
    return true;
}

//----------------------------------------------------------------------------
// CVisualWord

bool CVisualWord::Init(const CSynWord& piWord, const CSentence& piSentence) {
    try {
        m_strWord = FromRMLEncode(piWord.m_strWord);
        if (piWord.m_bDeleted) {
            m_strWord = "[" + m_strWord + "]";
        }
        m_bInTermin = piWord.m_bInTermin;
        m_bArtificialCreated = piWord.m_bArtificialCreated;
        m_iClauseNo = piWord.m_iClauseNo;

        // all lemmas and grammems
        for (int i = 0; i < (int)piWord.GetHomonymsCount(); i++) {
            const CSynHomonym& piHomonym = piWord.GetSynHomonym(i);
            CVisualHomonym hom;
            hom.m_strLemma = FromRMLEncode(piHomonym.GetLemma());
            hom.m_strCommonGrammems = FromRMLEncode(piSentence.GetOpt()->GetGramTab()->GrammemsToStr(piHomonym.m_TypeGrammems));
            hom.m_strPOS = FromRMLEncode(piHomonym.GetPartOfSpeechStr());
            if (piHomonym.m_bOborot1) {
                hom.m_strSomeDescr += " <Ob1>";
                if (piHomonym.m_OborotNo != -1) {
                    hom.m_strOborotsNum += "Ob: " + FromRMLEncode(piHomonym.GetOborotPtr()->m_OborotEntryStr);
                }
            }
            if (piHomonym.IsOb1()) {
                hom.m_strSomeDescr += " <InOb>";
            }
            if (piHomonym.IsOb2()) {
                hom.m_strSomeDescr += " <Ob2>";
            }
            if (!piHomonym.m_SimplePrepNos.empty()) {
                hom.m_strSomeDescr = " (All preps:";
                for (auto prep_no : piHomonym.m_SimplePrepNos) {
                    hom.m_strSomeDescr += ": " + FromRMLEncode(piSentence.GetOpt()->GetOborDic()->m_Entries[prep_no].m_OborotEntryStr);
                }
                hom.m_strSomeDescr += ")";
            }
            hom.m_iPradigmID = piHomonym.m_lPradigmID;
            m_arrHomonyms.push_back(hom);
        }
        if (m_arrHomonyms.empty()) {
            return false;
        }
        m_bBold = m_arrHomonyms.size() > 1;
        m_MainVerbs.assign(piWord.m_MainVerbs.begin(), piWord.m_MainVerbs.end());
    }
    catch (...) {
        return false;
    }
    return true;
}

int CVisualWord::GetWordLen(const QFontMetrics& fm) const {
    return fm.horizontalAdvance(m_strWord);
}

int CVisualWord::CalculateCoordinates(const QFontMetrics& fm, int iX, int iY) {
    int width = fm.horizontalAdvance(m_strWord);
    if (m_arrHomonyms.size() > 1) {
        // the word is printed in bold
        width = QFontMetrics(g_VisualFonts.m_Bold).horizontalAdvance(m_strWord);
    }
    m_WordRect.set(iX, iY, iX + width, iY + fm.height());
    return iX + width;
}

void CVisualWord::PrintWord(QPainter& p) const {
    p.save();
    if (m_bInTermin) {
        p.setPen(QColor(0, 0, 255));
    }
    else if (m_bArtificialCreated) {
        p.setPen(QColor(255, 0, 0));
    }
    bool bold = m_bBold || m_bArtificialCreated;
    const CVisualHomonym& hom = ActiveHomonym();
    if (hom.m_bSubj || hom.m_bPredk) {
        p.setFont(bold ? g_VisualFonts.m_BoldUnderline : g_VisualFonts.m_Underline);
    }
    else if (bold) {
        p.setFont(g_VisualFonts.m_Bold);
    }
    if (hom.m_bPredk) {
        p.save();
        p.setPen(Qt::black);
        p.drawLine(m_WordRect.left, m_WordRect.bottom + 2, m_WordRect.right, m_WordRect.bottom + 2);
        p.restore();
    }
    textOut(p, m_WordRect.left, m_WordRect.top, m_strWord);
    p.restore();
}

bool CVisualWord::SetActiveHomonym(int iActiveHomonim) {
    if (iActiveHomonim < 0 || iActiveHomonim >= (int)m_arrHomonyms.size()) {
        return false;
    }
    if (m_iActiveHomonym == iActiveHomonim) {
        return false;
    }
    m_iActiveHomonym = iActiveHomonim;
    return true;
}

void CVisualWord::ResetSubjAndPred() {
    for (auto& h : m_arrHomonyms) {
        h.m_bSubj = false;
        h.m_bPredk = false;
    }
}

// lemma and grammatical characteristics of the active homonym (for the tooltip)
QString CVisualWord::GetActiveHomDescr() const {
    const CVisualHomonym& hom = ActiveHomonym();
    QString s = hom.m_strLemma + " " + hom.m_strPOS + " " + hom.m_strCommonGrammems + " " + m_strActiveGrammems
        + " " + QString::number(hom.m_iPradigmID) + hom.m_strSomeDescr
        + QString(" (ClauseNo %1)").arg(m_iClauseNo) + m_strSomeDescr;
    return s;
}

//----------------------------------------------------------------------------
// CVisualSentence

CVisualSentence::~CVisualSentence() {
    for (auto c : m_arrClauses) {
        delete c;
    }
    for (auto w : m_arrWords) {
        delete w;
    }
}

bool CVisualSentence::Init(const CSentence& piSentence) {
    return ReadWords(piSentence) && ReadClauses(piSentence);
}

bool CVisualSentence::ReadWords(const CSentence& piSentence) {
    try {
        for (const auto& w : piSentence.GetWords()) {
            auto pVisualWord = new CVisualWord;
            if (!pVisualWord->Init(w, piSentence)) {
                delete pVisualWord;
                return false;
            }
            m_arrWords.push_back(pVisualWord);
        }
    }
    catch (...) {
        return false;
    }
    return true;
}

bool CVisualSentence::ReadClauses(const CSentence& piSentence) {
    int ClauseNo = 0;
    try {
        if (m_arrWords.empty()) {
            return false;
        }
        int iClausesCount = (int)piSentence.GetClausesCount();
        for (; ClauseNo < iClausesCount; ClauseNo++) {
            auto pClause = new CVisualClause;
            pClause->SetWordsArray(&m_arrWords);
            if (!pClause->InitVisualClause(piSentence.GetClause(ClauseNo))) {
                delete pClause;
                ErrorMessage("VisualSynan", Format("Cannot read clause N %i", ClauseNo));
                return false;
            }
            m_arrClauses.push_back(pClause);
        }
        std::sort(m_arrClauses.begin(), m_arrClauses.end(), PeriodsCompare_sort);
    }
    catch (...) {
        ErrorMessage("VisualSynan", Format("An exception occured while reading clauses N %i", ClauseNo));
        return false;
    }
    return true;
}

int CVisualSentence::CalculateSpaceBetweenLines(const QFontMetrics& fm) {
    std::map<int, int> mapClauseNumTypeNum;
    for (size_t i = 0; i < m_arrClauses.size(); i++) {
        if (!m_arrClauses[i]->FillActiveGroupsArray(mapClauseNumTypeNum, (int)i)) {
            ErrorMessage(Format("Cannot run function FillActiveGroupsArray for clause number %zu", i));
            return 0;
        }
    }
    int tmHeight = fm.height();
    int iGroupTextHeight = std::abs((tmHeight / 3) * 2);
    FillActiveGroupsArray();
    return m_iMaxGroupLevel * (iGroupTextHeight + RADIUS_ANGLE) + tmHeight + iGroupTextHeight;
}

void CVisualSentence::FillActiveGroupsArray() {
    m_arrActiveGroups.clear();
    for (auto pClause : m_arrClauses) {
        pClause->m_bClause = true;
        m_arrActiveGroups.push_back(pClause);
        for (auto g : pClause->m_arrActiveGroups) {
            m_arrActiveGroups.push_back(g);
        }
    }
    std::sort(m_arrActiveGroups.begin(), m_arrActiveGroups.end(), PeriodsCompare_sort);
    m_iMaxClausesLevel = AssignPeriodLevel(m_arrActiveGroups);
    m_iMaxGroupLevel = m_iMaxClausesLevel;
    m_pWordsArr = &m_arrWords;
}

int CVisualSentence::CalculateCoordinates(const QFontMetrics& fm, int iY, int iWidth) {
    m_iSpaceBetweenLinesG = CalculateSpaceBetweenLines(fm);
    int iPrevY = iY + SPACE_BETWEEN_SENTENCE;
    int iFinalY = iY + m_iSpaceBetweenLinesG + SPACE_BETWEEN_SENTENCE;
    CSentenceLine SentLine;
    m_vectorSentLines.clear();
    int iX = 0;
    int maxLen = 0;
    int spaceWidth = fm.horizontalAdvance(' ');
    for (size_t i = 0; i < m_arrWords.size(); i++) {
        CVisualWord* pVisualWord = m_arrWords[i];
        int iLen = pVisualWord->GetWordLen(fm);
        if (iX + iLen + spaceWidth > iWidth) {
            SentLine.m_rectLine.set(0, iPrevY, iWidth, iFinalY);
            iPrevY = iFinalY + SPACE_BETWEEN_SENTENCE;
            m_vectorSentLines.push_back(SentLine);
            SentLine.Reset();
            // carriage return
            iX = 0;
            iFinalY += m_iSpaceBetweenLinesG + SPACE_BETWEEN_SENTENCE;
        }
        // the rect of the whole sentence
        if (iX + iLen + spaceWidth >= maxLen) {
            maxLen = iX + iLen;
        }
        SentLine.m_vectorWordsNum.push_back((int)i);
        iX = pVisualWord->CalculateCoordinates(fm, iX, iFinalY - fm.height());
        iX += spaceWidth;
    }
    SentLine.m_rectLine.set(0, iPrevY, iWidth, iFinalY);
    m_vectorSentLines.push_back(SentLine);
    m_SentRect.set(0, iY, maxLen, iFinalY + fm.height());

    int iLine = 0;
    int iBottom = m_arrWords[0]->m_WordRect.bottom;
    CalculateGroupsCoordinates(fm, iWidth, iLine, iBottom);
    FillLinesVector();
    return iFinalY;
}

void CVisualSentence::FillLinesVector() {
    for (auto& line : m_vectorSentLines) {
        if (!line.m_vectorWordsNum.empty()) {
            CVisualWord* pWord = m_arrWords[line.m_vectorWordsNum[0]];
            int bottom = pWord->m_WordRect.bottom;
            FillClauseLine(line.m_vectorClauseArc, bottom - m_iSpaceBetweenLinesG, bottom);
        }
    }
}

void CVisualSentence::FillClauseLine(CFigureVector& vectorFigure, int lineTop, int lineBottom) {
    for (size_t i = 0; i < m_arrActiveGroups.size(); i++) {
        const CVisualGroup* pClause = m_arrActiveGroups[i];
        for (size_t j = 0; j < pClause->m_vectorParts.size(); j++) {
            int y = pClause->m_vectorParts[j].m_pointEnd.y();
            if (y >= lineTop && y <= lineBottom) {
                vectorFigure.push_back(SFigure((int)i, (int)j));
            }
        }
    }
}

void CVisualSentence::PrintAllLines(QPainter& p) const {
    for (size_t i = 0; i < m_vectorSentLines.size(); i++) {
        PrintLine(p, (int)i);
    }
}

void CVisualSentence::PrintLine(QPainter& p, int iLine) const {
    const CSentenceLine& SentLine = m_vectorSentLines[iLine];
    for (auto& figure : SentLine.m_vectorClauseArc) {
        m_arrActiveGroups[figure.m_iNum]->PrintGroupPart(p, figure.m_iPartNum);
    }
    for (int WordNo : SentLine.m_vectorWordsNum) {
        const CVisualWord* pWord = m_arrWords[WordNo];
        pWord->PrintWord(p);
        if (pWord->m_ReferenceWordNo != -1 && pWord->m_ReferenceWordNo < (int)m_arrWords.size()) {
            // an arc from a relative word to its antecedent
            const CVisualWord* pRefWord = m_arrWords[pWord->m_ReferenceWordNo];
            QPoint right(pRefWord->m_WordRect.left + pRefWord->m_WordRect.width() / 2, pRefWord->m_WordRect.top - 20);
            QPoint left(pWord->m_WordRect.right - pWord->m_WordRect.width() / 2, pWord->m_WordRect.top + 20);
            Rect R(left.x(), left.y(), right.x(), right.y());
            left.setY(pRefWord->m_WordRect.top);
            right.setY(pRefWord->m_WordRect.top);
            gdiArc(p, R, left, right);
            Rect pointer(right.x() - 3, right.y() - 3, right.x() + 3, right.y() + 3);
            gdiArc(p, pointer, right, right);
        }
    }
}

CVisualWord* CVisualSentence::GetWordByPoint(const QPoint& point, int* iActiveWord) {
    for (size_t i = 0; i < m_arrWords.size(); i++) {
        if (m_arrWords[i]->PointInWordRect(point)) {
            *iActiveWord = (int)i;
            return m_arrWords[i];
        }
    }
    return nullptr;
}

QString CVisualSentence::GetActiveHomDescr(int iActiveWord) const {
    if (iActiveWord < 0 || iActiveWord >= (int)m_arrWords.size()) {
        return QString();
    }
    return m_arrWords[iActiveWord]->GetActiveHomDescr();
}

bool CVisualSentence::SetActiveHomonym(int iActiveWord, int iActiveHomonim) {
    if (iActiveWord < 0 || iActiveWord >= (int)m_arrWords.size()) {
        return false;
    }
    return m_arrWords[iActiveWord]->SetActiveHomonym(iActiveHomonim);
}

//----------------------------------------------------------------------------
// CVisualSentences

void CVisualSentences::Reset() {
    for (auto s : m_arrSentences) {
        delete s;
    }
    m_arrSentences.clear();
}

bool CVisualSentences::FillSentencesArray(const CSentencesCollection& coll) {
    try {
        for (const auto& s : coll.m_vectorSents) {
            auto vs = new CVisualSentence;
            if (!vs->Init(*s)) {
                // a sentence without words or clauses cannot be shown
                delete vs;
                continue;
            }
            m_arrSentences.push_back(vs);
        }
    }
    catch (...) {
        return false;
    }
    return true;
}

void CVisualSentences::CalculateCoordinates(const QFontMetrics& fm, int iWidth) {
    int iHeight = 0;
    for (auto s : m_arrSentences) {
        iHeight = s->CalculateCoordinates(fm, iHeight, iWidth);
    }
}

void CVisualSentences::PrintSentences(QPainter& p) const {
    for (auto s : m_arrSentences) {
        s->PrintAllLines(p);
    }
}

CVisualWord* CVisualSentences::GetWordByPoint(const QPoint& point, int* iActiveSentence, int* iActiveWord) {
    for (size_t i = 0; i < m_arrSentences.size(); i++) {
        if (m_arrSentences[i]->PointInSentRect(point)) {
            if (auto w = m_arrSentences[i]->GetWordByPoint(point, iActiveWord)) {
                *iActiveSentence = (int)i;
                return w;
            }
        }
    }
    return nullptr;
}

bool CVisualSentences::SetActiveHomonym(int iActiveSentence, int iActiveWord, int iActiveHomonim) {
    if (iActiveSentence < 0 || iActiveSentence >= (int)m_arrSentences.size()) {
        return false;
    }
    return m_arrSentences[iActiveSentence]->SetActiveHomonym(iActiveWord, iActiveHomonim);
}

QString CVisualSentences::GetActiveHomDescr(int iActiveSentence, int iActiveWord) const {
    if (iActiveSentence < 0 || iActiveSentence >= (int)m_arrSentences.size()) {
        return QString();
    }
    return m_arrSentences[iActiveSentence]->GetActiveHomDescr(iActiveWord);
}

QSize CVisualSentences::GetSentencesSize() const {
    if (m_arrSentences.empty()) {
        return QSize(0, 0);
    }
    const Rect& r = m_arrSentences.back()->m_SentRect;
    return QSize(r.right, r.bottom);
}

//----------------------------------------------------------------------------

static QString wordsOfPeriod(const CSentence* piSent, int first, int last) {
    QString s;
    for (int k = first; k <= last; k++) {
        s += FromRMLEncode(piSent->GetWords()[k].m_strWord) + " ";
    }
    return s;
}

QString BuildRels(const CSentencesCollection& coll) {
    QString report;
    int sentNo = 0;
    for (const CSentence* piSent : coll.m_vectorSents) {
        report += QString("Sentence № %1\n").arg(++sentNo);
        CRelationsIterator iter;
        iter.SetSentence(piSent);
        for (int j = 0; j < (int)piSent->GetClausesCount(); j++) {
            iter.AddClauseNoAndVariantNo(j, 0);
        }
        iter.BuildRelations();

        report += "Firm groups: \n";
        for (const CGroup& g : iter.GetFirmGroups()) {
            report += FromRMLEncode(piSent->GetOpt()->GetGroupNameByIndex(g.m_GroupType))
                + "(" + wordsOfPeriod(piSent, g.m_iFirstWord, g.m_iLastWord) + ")\n";
        }

        report += "Relations: \n";
        for (const CSynOutputRelation& piRel : iter.GetRelations()) {
            QString str = FromRMLEncode(piSent->GetOpt()->GetGroupNameByIndex(piRel.m_Relation.type)) + "(";
            if (piRel.m_iSourceGroup == -1) {
                str += FromRMLEncode(piSent->GetWords()[piRel.m_Relation.m_iFirstWord].m_strWord);
            }
            else {
                const CGroup& g = iter.GetFirmGroups()[piRel.m_iSourceGroup];
                str += wordsOfPeriod(piSent, g.m_iFirstWord, g.m_iLastWord);
            }
            str += " -> ";
            if (piRel.m_iTargetGroup == -1) {
                str += FromRMLEncode(piSent->GetWords()[piRel.m_Relation.m_iLastWord].m_strWord);
            }
            else {
                const CGroup& g = iter.GetFirmGroups()[piRel.m_iTargetGroup];
                str += wordsOfPeriod(piSent, g.m_iFirstWord, g.m_iLastWord);
            }
            report += "  " + str + ")\n";
        }
    }
    return report;
}
