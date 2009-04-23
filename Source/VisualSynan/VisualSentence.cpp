#include "stdafx.h"
#include "VisualSentences.h"


CVisualSentence::CVisualSentence()
{
	m_SentRect.SetRectEmpty();
	m_iSpaceBetweenLinesG = 70;
}

CVisualSentence::~CVisualSentence()
{
	for(int i = 0 ; i < m_arrClauses.size(); i++ )
	{
		CVisualClause* pClause = m_arrClauses[i];
		if(pClause)
			delete pClause;
	}

	for(int i = 0 ; i < m_arrWords.GetSize(); i++ )
	{
		CVisualWord* pWord = (CVisualWord*)m_arrWords.GetAt(i);
		if(pWord)
			delete pWord;
	}
}


BOOL CVisualSentence::Init(ISentencePtr& piSentence)
{
	BOOL bRes;

	bRes = ReadWords(piSentence);
	if(!bRes)
		return FALSE;

	bRes = ReadClauses(piSentence);
	if(!bRes)
		return FALSE;

	return TRUE;


}



BOOL CVisualSentence::LineInrRect(CRect& rectForDrawing, CSentenceLine& line)
{
	CRect& rectLine = line.m_rectLine;
	return ((rectLine.bottom <= rectForDrawing.bottom) &&
			(rectLine.left >= rectForDrawing.left) &&
			//(rectLine.right <= rectForDrawing.right) &&
			(rectLine.top >= rectForDrawing.top));
}

int	CVisualSentence:: InRect(CRect& rectForDrawing)
{

	for(int i = 0 ; i < m_vectorSentLines.size() ; i++ )
	{
		
		if( LineInrRect(rectForDrawing, m_vectorSentLines[i]) )
			return i;
	}
	return -1;
}

int CVisualSentence::GetLineBottom(int i)
{
	return m_vectorSentLines[i].m_rectLine.bottom;
}


BOOL CVisualSentence::PrintAllLinesTillBottom(int& iStartLine,CDC*  pDC,int iBottom, int& iOffset) 
{
	for(int i = iStartLine ; i < m_vectorSentLines.size() ; i++ )
	{
		if(  m_vectorSentLines[i].m_rectLine.bottom <= iBottom )

			PrintLine(pDC, i, iOffset);
		else
			return FALSE;
	}
	return TRUE;
}

BOOL CVisualSentence::PrintAllLinesInRect(int iStartLine, CDC* pDC, CRect& rectForDrawing, int iOffset)
{
	int iOffset1 = iOffset;
	for(int i = iStartLine ; i < m_vectorSentLines.size() ; i++ )
	{
		if( LineInrRect(rectForDrawing, m_vectorSentLines[i]) )
			PrintLine(pDC, i, iOffset1);
		else
			return FALSE;
		iOffset1 = iOffset;
	}

	return TRUE;
}



int CVisualSentence::CalculateCoordinates(CDC* pDC,int iY,int iWidth,BOOL bShowGroups)
{
	m_iSpaceBetweenLinesG = CalculateSpaceBetweenLines(pDC, bShowGroups);
	int iPrevY = iY + SPACE_BETWEEN_SENTENCE;
	int iFinalY = iY + m_iSpaceBetweenLinesG + SPACE_BETWEEN_SENTENCE;
	CSentenceLine SentLine;

	SentLine.m_bShowGroups = bShowGroups;
	m_vectorSentLines.clear();

	try
	{

		int iX = 0;
		CVisualWord* pVisualWord;
		int iLen, maxLen = 0;
		CSize sizeSpace;
		sizeSpace = pDC->GetOutputTextExtent(" ");
		
		for(int i = 0 ; i < m_arrWords.GetSize() ; i++)
		{
			pVisualWord = (CVisualWord*)(m_arrWords.GetAt(i));
			iLen = pVisualWord->GetWordLen(pDC);			
			if( (iX + iLen + sizeSpace.cx) > iWidth )
			{				
				SentLine.m_rectLine.SetRect(0, iPrevY, iWidth, iFinalY);
				iPrevY = iFinalY + SPACE_BETWEEN_SENTENCE;
				m_vectorSentLines.push_back(SentLine);
				SentLine.Reset();
				//возврат каретки
				iX = 0;
				iFinalY += m_iSpaceBetweenLinesG + SPACE_BETWEEN_SENTENCE;
			}
			
			//для вычисления rect для всего предложения
			if( (iX + iLen + sizeSpace.cx) >= maxLen )
				maxLen = iX + iLen;

			SentLine.m_vectorWordsNum.push_back(i);

			int iWHeight = pVisualWord->GetWordHight(pDC);

			iX = pVisualWord->CalculateCoordinates(pDC,iX,iFinalY - iWHeight);			
			iX += sizeSpace.cx;
		}

		SentLine.m_rectLine.SetRect(0, iPrevY, iWidth, iFinalY);
		m_vectorSentLines.push_back(SentLine);

		TEXTMETRIC txtM;
		pDC->GetTextMetrics(&txtM);

		m_SentRect.SetRect(0 , iY, maxLen, iFinalY + txtM.tmHeight);
		
		int iLine = 0;
		int iBottom = ((CVisualWord*)m_arrWords.GetAt(0))->m_WordRect.bottom;
		CalculateGroupsCoordinates(pDC, iWidth, iLine,iBottom);
		
		FillLinesVector(bShowGroups);
	}
	catch(...)
	{
		return -1;
	}
	return iFinalY;
}


void CVisualSentence::FillLinesVector(BOOL bShowGroups)
{
	int j = 0, k = 0, l = 0;
	int iLine = 0, iClausePart = 0, iGroupPart = 0, iRelPart = 0, iClauseNum = 0, iRelNum = 0, iGroupNum = 0;	

	for(int i = 0 ; i < m_vectorSentLines.size() ; i++ )
	{		
		//assert (!m_vectorSentLines[i].m_vectorWordsNum.empty());
		if (!m_vectorSentLines[i].m_vectorWordsNum.empty())
		{
			int iWord = m_vectorSentLines[i].m_vectorWordsNum[0];
			CVisualWord* pWord = ((CVisualWord*)m_arrWords.GetAt(iWord));
			CIntPair LineTopAndBot;
			LineTopAndBot.second = pWord->m_WordRect.bottom;
			LineTopAndBot.first = LineTopAndBot.second - m_iSpaceBetweenLinesG;
			ASSERT(LineTopAndBot.first >= 0);

			m_vectorSentLines[i].m_bShowGroups = bShowGroups;

			FillClauseLine(m_vectorSentLines[i].m_vectorClauseArc, LineTopAndBot);
		}

	}
}

void CVisualSentence::FillClauseLine(CFigureVector& vectorFigure, CIntPair& LineTopAndBot)
{
	for(int i = 0; i < m_arrActiveGroups.size() ; i++ )
	{			
		CVisualGroup* pClause = m_arrActiveGroups[i]; 
		for(int j = 0 ; j < pClause->m_vectorParts.size() ; j++ )
		{
			if( (pClause->m_vectorParts[j]->m_pointEnd.y >= LineTopAndBot.first) && (pClause->m_vectorParts[j]->m_pointEnd.y <= LineTopAndBot.second) )
				vectorFigure.push_back(SFigure(i,j));
		}

	}
	
}




void CVisualSentence::PrintAllLines(CDC* pDC)
{
	int iOffset = 0;
	for(int i = 0 ; i < m_vectorSentLines.size() ; i++ )
		PrintLine(pDC, i, iOffset);
	
}

void CVisualSentence::PrintLine(CDC *pDC, int iLine, int& iOffset)
{
	CSentenceLine& SentLine = m_vectorSentLines[iLine];
	int i;

	for(i = 0 ; i < SentLine.m_vectorClauseArc.size() ; i++ )
	{
		SFigure& Figure = SentLine.m_vectorClauseArc[i];
		CVisualGroup* pClause  = m_arrActiveGroups[Figure.m_iNum];		
		pClause->PrintGroupPart(pDC, Figure.m_iPartNum, iOffset);
	}

	for(i = 0 ; i < SentLine.m_vectorWordsNum.size() ; i++ )
	{
		int WordNo = SentLine.m_vectorWordsNum[i];
		CVisualWord* pWord = (CVisualWord*)m_arrWords.GetAt(WordNo);
		pWord->PrintWord(pDC, iOffset);
		if (pWord->m_ReferenceWordNo != -1)
		{
			CVisualWord* pRefWord = (CVisualWord*)m_arrWords.GetAt(pWord->m_ReferenceWordNo);
			CPoint right = pRefWord->m_WordRect.TopLeft();
			right.y -= 20+iOffset;
			right.x += pRefWord->m_WordRect.Size().cx/2;
			CPoint left;
			left.x = pWord->m_WordRect.right-pWord->m_WordRect.Size().cx/2;
			left.y = pWord->m_WordRect.top - iOffset + 20;
			CRect R(left, right);
			left.y = right.y = pRefWord->m_WordRect.top - iOffset;
			
			pDC->Arc(R, left, right);
			CRect pointer(right,right);
			pointer.top -= 3;
			pointer.left -= 3;
			pointer.bottom += 3;
			pointer.right += 3;
			pDC->Arc(pointer, pointer.CenterPoint(), pointer.CenterPoint());
			

		};
	}
}


BOOL CVisualSentence::ReadClauses(ISentencePtr& piSentence)
{

	int ClauseNo = 0;
	try
	{
		int iClausesCount = piSentence->GetClausesCount();

		if( !m_arrWords.GetSize() )
			return FALSE;
		
		for(; ClauseNo < iClausesCount ; ClauseNo++ )
		{

			CVisualClause* pClause = new CVisualClause;
			IClausePtr piClause = piSentence->GetClause(ClauseNo);			

			pClause->SetWordsArray(&m_arrWords);

			if( !pClause->InitVisualClause(piClause ) )
			{	
				ErrorMessage("VisualSynan", Format("Cannot read clause N %i",ClauseNo));
				return FALSE;
			};


			
			m_arrClauses.push_back(pClause);		
		}

		sort(m_arrClauses.begin(), m_arrClauses.end(), PeriodsCompare_sort);

	}
	catch(...)
	{
		ErrorMessage("VisualSynan", Format("An exception occured while reading clauses N %i",ClauseNo));
		return FALSE;
	}

	return TRUE;
}

BOOL CVisualSentence::ReadWords(ISentencePtr& piSentence)
{
	try
	{
		BOOL bRes = TRUE;
		long lWordCount;
		lWordCount = piSentence->GetWordsNum();
		IWordPtr piWord;
		CVisualWord* pVisualWord;

		for(int i = 0 ; i < lWordCount ; i++)
		{
			piWord = piSentence->GetWord(i);
			pVisualWord = new CVisualWord;
			bRes = pVisualWord->Init(piWord, piSentence);
			if( !bRes)
				return FALSE;
			m_arrWords.Add(pVisualWord);
		}
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}


int CVisualSentence::CalculateSpaceBetweenLines(CDC* pDC, BOOL bShowGroups)
{	
	int iMaxLevel = 0;
	map<int, int> mapClauseNumTypeNum;

	for(int i = 0 ; i < m_arrClauses.size() ; i++ )
	{
		CVisualClause* pClause = m_arrClauses[i];
		if( !pClause->FillActiveGroupsArray(TRUE, mapClauseNumTypeNum, i) )
		{
			::ErrorMessage(Format("Cannot run function  FillActiveGroupsArray fo Clause Number %i",i));
			return FALSE;		
		};
	}
	
	TEXTMETRIC txtMetr;
	pDC->GetTextMetrics(&txtMetr);
	int iGroupTextHeight = abs ((txtMetr.tmHeight/3)*2 );
	ASSERT(txtMetr.tmHeight > 0);

	FillActiveGroupsArray(TRUE);

	return (m_iMaxGroupLevel)*(iGroupTextHeight + RADIUS_ANGLE) + txtMetr.tmHeight + iGroupTextHeight;
}


BOOL CVisualSentence::FillActiveGroupsArray(BOOL bUseGroupID)
{

	int ii = 0;
	m_arrActiveGroups.clear();
	for(int i = 0 ; i < m_arrClauses.size() ; i++ )
	{		
		CVisualClause* pClause = m_arrClauses[i];
		m_arrActiveGroups.push_back((CVisualGroup*)pClause);
		m_arrActiveGroups[m_arrActiveGroups.size() - 1]->m_bClause = TRUE;
		for(int k = 0 ; k < pClause->m_arrActiveGroups.size() ; k++ )		
			m_arrActiveGroups.push_back(pClause->m_arrActiveGroups[k]);
				
	}

	sort(m_arrActiveGroups.begin(), m_arrActiveGroups.end(), PeriodsCompare_sort);
	m_iMaxClausesLevel = ::AssignPeriodLevel(m_arrActiveGroups);
	m_iMaxGroupLevel = m_iMaxClausesLevel;


	m_pWordsArr = &m_arrWords;

	return TRUE;

}

BOOL CVisualSentence::GetHomonymsArray(CPoint& point,CPtrArray** pHomonymsArray, int* iActiveWord)
{
	CVisualWord* pVisualWord;
	
	for(int i = 0 ; i < m_arrWords.GetSize() ; i++)
	{
		pVisualWord = (CVisualWord*)(m_arrWords.GetAt(i));
		if(pVisualWord->PointInWordRect(point))
		{			
			pVisualWord->GetHomonymsArray(pHomonymsArray); 
			*iActiveWord = i;				
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CVisualSentence::GetActiveHomDescr(int iActiveWord,CString& strLemma,CString& strGramChar)
{
	if( (iActiveWord < 0) || (iActiveWord >= m_pWordsArr->GetSize()) )
		return FALSE;

	return ((CVisualWord*)(m_pWordsArr->GetAt(iActiveWord )))->GetActiveHomDescr(strLemma,strGramChar);
}

BOOL CVisualSentence::SetActiveHomonym(int iActiveWord, int iActiveHomonim)
{
	if( (iActiveWord < 0) || (iActiveWord >= m_pWordsArr->GetSize()) )
		return FALSE;


	return ((CVisualWord*)(m_pWordsArr->GetAt(iActiveWord )))->SetActiveHomonym(iActiveHomonim);
}

