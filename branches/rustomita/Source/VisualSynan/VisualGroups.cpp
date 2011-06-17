#include "stdafx.h"
#include "VisualSentences.h"


void  CVisualGroups::CalculateGroupsCoordinates(CDC* pDC,int iWidth, int& iLine, int iPrevBottom)
{

	COLORREF Color;
	CVisualGroup* pGroup;
	BOOL bRes;

	int iWordsCount = m_pWordsArr->GetSize();
	TEXTMETRIC txtM;
	pDC->GetTextMetrics(&txtM);	
	int iBottom;	
		
	for(int i = 0 ; i < m_arrActiveGroups.size() ; i++)
	{
		
		pGroup = m_arrActiveGroups[i];
		if( pGroup->m_bClause )
			Color = RGB(255,0,0);
		else
			Color = RGB(0,0,255);
		
		int iFirstWord, iLastWord;
		iFirstWord = pGroup->m_iFirstWord;
		iLastWord =  pGroup->m_iLastWord;
		
		
		CRect& rectFirstWord = ((CVisualWord*)(m_pWordsArr->GetAt(iFirstWord)))->m_WordRect;
		CRect& rectLastWord = ((CVisualWord*)(m_pWordsArr->GetAt(iLastWord)))->m_WordRect;

		iBottom = min(rectFirstWord.bottom, rectLastWord.bottom);

		BOOL bOnDifferentLines = (rectFirstWord.bottom != rectLastWord.bottom);

		if( pGroup->m_iLevel == 0 )
			return;
		int yCoef = ((m_iSpaceBetweenLinesG - (txtM.tmHeight + (txtM.tmHeight/3)*2))/(m_iMaxGroupLevel) ) * (m_iMaxGroupLevel -  pGroup->m_iLevel);
		

		CPoint pointLeftLeg;
		bRes = GetLeftLegPointForGroupArc(i,&pointLeftLeg,iWidth);
		if( !bRes )
			return;

		CPoint pointRightLeg;
		bRes = GetRightLegPointForGroupArc(i,&pointRightLeg,iWidth);
		if( !bRes )
			return;
		int top  = rectFirstWord.top - (m_iSpaceBetweenLinesG - (txtM.tmHeight + (txtM.tmHeight/3)*2)) + yCoef;
		
		if( bOnDifferentLines )
		{
			bOnDifferentLines = !( (pointLeftLeg.y < rectFirstWord.bottom) && (pointRightLeg.y < rectFirstWord.bottom) );
		}

		if( ( iBottom > iPrevBottom ) && !bOnDifferentLines )
			iLine++;
		
		pGroup->SetSpaceBetweenLines(m_iSpaceBetweenLinesG);
		pGroup->CalculateCoordinates(pDC,pointLeftLeg,pointRightLeg,top,iWidth,bOnDifferentLines, Color, iLine);

		iPrevBottom = min(rectFirstWord.bottom, rectLastWord.bottom);		
	}
	
}



BOOL  CVisualGroups::GetLeftLegPointForGroupArc(int iGroupNum, CPoint* pPoint, int iWidth)
{
	CPoint point;
	CVisualGroup* pGroupForDrawing = m_arrActiveGroups[iGroupNum];	
	BOOL bExistUsefulGroup = FALSE;
	CVisualGroup* pGroup;

	int i = iGroupNum - 1;
	for( ; i >= 0 ; i-- )
	{
		pGroup = m_arrActiveGroups[i];
		if(pGroup->m_iFirstWord == pGroupForDrawing->m_iFirstWord)
		{
			bExistUsefulGroup = TRUE;
			break;
		}		
	}

	if(		bExistUsefulGroup 
		&& 	!(pGroupForDrawing->m_bClause && !m_arrActiveGroups[i]->m_bClause) 
	  )
	{	
		ASSERT( i >= 0 );
		pGroup = m_arrActiveGroups[i];
		//from the middle
		if((pGroup->m_pointLeft.x  == 0) && (pGroup->m_pointRight.x))
			return FALSE;

		pPoint->y = pGroup->m_pointLeft.y;

		if(pGroup->m_pointLeft.y == pGroup->m_pointRight.y)
		{
			//! pPoint->x = pGroup->m_pointLeft.x + (pGroup->m_pointRight.x - pGroup->m_pointLeft.x)/2;
			pPoint->x = pGroup->m_pointLeft.x + 10;
			
		}
		//if this group is on different lines
		else
		{
			//!s pPoint->x = pGroup->m_pointLeft.x + (iWidth - pGroup->m_pointLeft.x)/2;
			pPoint->x = pGroup->m_pointLeft.x + 10;
		}
	}
	else
	{
		CVisualWord* pWord = (CVisualWord* )(m_pWordsArr->GetAt(pGroupForDrawing->m_iFirstWord));
		if( pWord->m_WordRect.IsRectEmpty() )
			return FALSE;
		//from the middle of the word
		//pPoint->x = pWord->m_WordRect.left + (pWord->m_WordRect.right - pWord->m_WordRect.left)/2;	
		pPoint->x = pWord->m_WordRect.left + 10;	
		pPoint->y = pWord->m_WordRect.top;
	}
	return TRUE;
}


BOOL CVisualGroups::GetRightLegPointForGroupArc(int iGroupNum, CPoint* pPoint, int iWidth)
{
	CVisualGroup* pGroupForDrawing = m_arrActiveGroups[iGroupNum];
	CVisualGroup* pGroup;
	BOOL bExistUsefulGroup = FALSE;	
	if( iGroupNum > 0 )
	{
		pGroup = m_arrActiveGroups[iGroupNum - 1];
		if(pGroup->m_iLastWord == pGroupForDrawing->m_iLastWord)
			bExistUsefulGroup = TRUE;
	}

	if(		bExistUsefulGroup 
		&& 	!(pGroupForDrawing->m_bClause && !pGroup->m_bClause) 
	  )
	{
		pPoint->y = pGroup->m_pointLeft.y;

		if(pGroup->m_pointLeft.y == pGroup->m_pointRight.y)
		{
			//s pPoint->x = pGroup->m_pointLeft.x + (pGroup->m_pointRight.x - pGroup->m_pointLeft.x)/2;
			pPoint->x = pGroup->m_pointRight.x-10;
			
		}
		//if this group is on different lines
		else
		{
			//s pPoint->x = pGroup->m_pointLeft.x + (iWidth - pGroup->m_pointLeft.x)/2;
			pPoint->x = pGroup->m_pointRight.x + iWidth - 10;

		}

	}
	else
	{
		CVisualWord* pWord = (CVisualWord* )(m_pWordsArr->GetAt(pGroupForDrawing->m_iLastWord));
		if( pWord->m_WordRect.IsRectEmpty() )
			return FALSE;
		//from the middle of the word
		//s pPoint->x = pWord->m_WordRect.left + (pWord->m_WordRect.right - pWord->m_WordRect.left)/2;	
		pPoint->x = pWord->m_WordRect.right-10;	
		pPoint->y = pWord->m_WordRect.top;	
	}
	return TRUE;
}
