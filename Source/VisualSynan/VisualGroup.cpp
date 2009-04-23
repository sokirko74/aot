#include "stdafx.h"
#include "VisualSentences.h"
#include "VisualSynanView.h"


CVisualGroup::CVisualGroup()
{
	m_pointLeft.x = 0;
	m_pointLeft.y = 0;
	m_pointRight.x = 0;
	m_pointRight.y = 0;
	m_iFirstWord = 0;
	m_iLastWord = 0;
	m_iLevel = 0;
	m_iSpaceBetweenLines = 70;
	m_bClause = FALSE;
}


BOOL CVisualGroup::Init(IGroupPtr& piGroup)
{
	
	m_iFirstWord = piGroup->GetFirstWord();
	m_iLastWord = piGroup->GetLastWord();
	_bstr_t bstrDescr= piGroup->TypeStr;
	m_strDescription = CString((char*)bstrDescr);
	m_strDescription.MakeLower();
	return TRUE;
}





BOOL CVisualGroup::CalculateCoordinates(CDC* pDC, CPoint& pointLeftLeg, CPoint& poitRightLeg, int top, int iWidth, BOOL bOnDifferentLines, COLORREF Color, int iLine)
{
	
	ResetParts();

	CPartOfGrArc* pPartOfGrArc;
	pPartOfGrArc = NewGroupArcPart(Color, iLine, 
									pointLeftLeg, 
									CPoint(pointLeftLeg.x, top + RADIUS_ANGLE /*- 2*/),
									Line, CRect(0,0,0,0),FALSE, FALSE);

	m_vectorParts.push_back(pPartOfGrArc);	

	pPartOfGrArc = NewGroupArcPart(Color, iLine, 
									CPoint(pointLeftLeg.x + RADIUS_ANGLE, top), 
									CPoint(pointLeftLeg.x, top + RADIUS_ANGLE + 2),
									ArcFig, CRect(pointLeftLeg.x, top , pointLeftLeg.x + 2*RADIUS_ANGLE - 2, top + 2*RADIUS_ANGLE),FALSE, FALSE);

	m_vectorParts.push_back(pPartOfGrArc);	



	CPoint _Point;

	//if groups are on different lines
	if( bOnDifferentLines)
	{


		pPartOfGrArc = NewGroupArcPart(Color, iLine, 
										CPoint(pointLeftLeg.x + RADIUS_ANGLE, top), 
										CPoint(iWidth, top),
										Line, CRect(0,0,0,0),FALSE, FALSE);

		m_vectorParts.push_back(pPartOfGrArc);

		iLine++;

		int k = 1;
		
		while( poitRightLeg.y - top > (k+1)*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) )
		{

			pPartOfGrArc = NewGroupArcPart(Color, iLine, 
											CPoint(0,top + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) ),
											CPoint(iWidth, top + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) ),
											Line, CRect(0,0,0,0) ,TRUE, FALSE);
			
			m_vectorParts.push_back(pPartOfGrArc);		
			k++;
		}

		pPartOfGrArc = NewGroupArcPart(Color, iLine, 
										CPoint(0,top + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) ),
										CPoint(poitRightLeg.x - RADIUS_ANGLE + 1, top + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) ),
										Line, CRect(0,0,0,0) ,TRUE, FALSE);
		
		m_vectorParts.push_back(pPartOfGrArc);		


		pPartOfGrArc = NewGroupArcPart(Color, iLine, 										
										CPoint(poitRightLeg.x , top + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) + RADIUS_ANGLE),
										CPoint(poitRightLeg.x - RADIUS_ANGLE + 1,top + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) - 1),
										ArcFig,CRect(poitRightLeg.x - 2*RADIUS_ANGLE, top + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE), poitRightLeg.x + 1, top + 2*RADIUS_ANGLE + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) ) ,FALSE, FALSE);

		m_vectorParts.push_back(pPartOfGrArc);		



		_Point.x = poitRightLeg.x;
		_Point.y = top + k*(m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE) +  RADIUS_ANGLE;

	}
	else
	{
		pPartOfGrArc = NewGroupArcPart(Color, iLine, 
										CPoint(pointLeftLeg.x + RADIUS_ANGLE, top),
										CPoint(poitRightLeg.x - RADIUS_ANGLE + 1, top),
										Line, CRect(0,0,0,0),TRUE, TRUE);

		m_vectorParts.push_back(pPartOfGrArc);		

		pPartOfGrArc = NewGroupArcPart(Color, iLine, 
										CPoint(poitRightLeg.x +1, top + RADIUS_ANGLE),
										CPoint(poitRightLeg.x - RADIUS_ANGLE + 1,top ),
										ArcFig,CRect(poitRightLeg.x - 2*RADIUS_ANGLE + 3, top, poitRightLeg.x + 1, top + 2*RADIUS_ANGLE ) ,FALSE, TRUE);

		m_vectorParts.push_back(pPartOfGrArc);		



		_Point.x = poitRightLeg.x;
		_Point.y = top +  RADIUS_ANGLE;		
	}

	pPartOfGrArc = NewGroupArcPart(Color, iLine, 
									_Point, 
									poitRightLeg,
									Line, CRect(0,0,0,0),FALSE, FALSE);

	m_vectorParts.push_back(pPartOfGrArc);		

	m_pointLeft.x = pointLeftLeg.x;
	m_pointLeft.y = top;
	m_pointRight.x = poitRightLeg.x;

	if( bOnDifferentLines )
		m_pointRight.y = top + m_iSpaceBetweenLines + SPACE_BETWEEN_SENTENCE;
	else
		m_pointRight.y = top;
	return TRUE;
}

void CVisualGroup::PrintGroupPart(CDC* pDC, int i, int iOffset)
{
	if( (i < 0) || (i >= m_vectorParts.size()) )
		return;

	CPartOfGrArc* pPart = reinterpret_cast<CPartOfGrArc*>(m_vectorParts[i]);

	ASSERT( pPart != NULL );

	CPen penForArc(PS_SOLID, 1, pPart->m_Color);
	CPen* pOldPen = pDC->SelectObject(&penForArc);

	CPoint pointStart(pPart->m_pointStart.x, pPart->m_pointStart.y - iOffset );
	CPoint pointEnd(pPart->m_pointEnd.x, pPart->m_pointEnd.y - iOffset);
	CRect  rectForArc(pPart->m_RectForArc);
	rectForArc.bottom -= iOffset;
	rectForArc.top -= iOffset;

	if( pPart->type == ArcFig )
	{
		//CBrush br(pPart->m_Color);
		pDC->Arc(rectForArc, pointStart, pointEnd);//AngleArc( pointEnd.x, pointEnd.y, RADIUS_ANGLE, pPart->m_iAngle1, pPart->m_iAngle2);	
		//pDC->FrameRect(&rectForArc, &br);

	}

	if( pPart->type == Line )
	{
		pDC->MoveTo(pointStart);
		pDC->LineTo(pointEnd);
	}

	pDC->SelectObject(pOldPen);

	if( pPart->m_bHasDescription )
	{
		CFont* pOldFont = pDC->GetCurrentFont(); 

		pDC->SelectObject(& (CVisualSynanView::m_FontForGroupNames));

		TEXTMETRIC txtM;
		pDC->GetTextMetrics(&txtM);


		if( !pPart->m_bWholeArc )
		{
			pDC->TextOut(5,pPart->m_pointStart.y - txtM.tmHeight - 2 - iOffset, m_strDescription, m_strDescription.GetLength() );
		}
		else
		{
			pDC->TextOut(m_pointLeft.x + (m_pointRight.x - m_pointLeft.x)/2 +2 ,pPart->m_pointStart.y - txtM.tmHeight - 2 - iOffset, m_strDescription, m_strDescription.GetLength());
		}

		pDC->SelectObject(pOldFont);
		
	}
}




CPartOfGrArc* NewGroupArcPart(long color, int line, CPoint start, CPoint end,ETypeFigure type, CRect rectForArc, BOOL has_descr, BOOL whole_arc)
{
	CPartOfGrArc* pPartOfGrArc = new CPartOfGrArc;

	pPartOfGrArc->m_Color = color;
	pPartOfGrArc->m_iLine = line;
	pPartOfGrArc->m_pointStart = start;
	pPartOfGrArc->type = type;
	pPartOfGrArc->m_pointEnd = end;
	pPartOfGrArc->m_RectForArc = rectForArc;
	pPartOfGrArc->m_bHasDescription = has_descr;
	pPartOfGrArc->m_bWholeArc = whole_arc;

	return pPartOfGrArc;
}


