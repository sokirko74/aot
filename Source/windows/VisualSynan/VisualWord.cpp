#include "StdAfx.h"
#include "VisualSentences.h"
#include "VisualSynanView.h"



CVisualWord::CVisualWord()
{
	m_WordRect.SetRectEmpty();
	m_iActiveHomonym = 0;
	m_bBold = FALSE;
	m_bInTermin = FALSE;
	m_bArtificialCreated = FALSE;
	m_ReferenceWordNo = -1;
}

CVisualWord::~CVisualWord()
{
	for(int i = 0 ; i < m_arrHomonyms.GetSize(); i++ )
	{
		CVisualHomonym* pHom;
		pHom = (CVisualHomonym*)m_arrHomonyms.GetAt(i);
		//delete[] pHom->m_arrGroupIDs;
		delete pHom;
	}
}

BOOL CVisualWord::Init(const CSynWord& piWord, const CSentence& piSentence)
{
	try
	{
		m_strWord = FromRMLEncode(piWord.m_strWord.c_str());
		if (piWord.m_bDeleted)
			m_strWord = "[" + m_strWord + "]";
		m_bInTermin = piWord.m_bInTermin;
		m_bArtificialCreated = piWord.m_bArtificialCreated;
		long lHomCount;

		m_iClauseNo = piWord.m_iClauseNo;
		//вытаскиваем все леммы и грамемы
		lHomCount = piWord.GetHomonymsCount();
		for(int i = 0 ; i < lHomCount ; i++) 
		{
			const CSynHomonym& piHomonym = piWord.GetSynHomonym(i);

			CVisualHomonym* pHomonym = new CVisualHomonym;
			pHomonym->m_strLemma = FromRMLEncode(piHomonym.m_strLemma);
			pHomonym->m_strCommonGrammems = FromRMLEncode(piSentence.GetOpt()->GetGramTab()->GrammemsToStr(piHomonym.m_TypeGrammems));
			pHomonym->m_strPOS = FromRMLEncode(piHomonym.GetPartOfSpeechStr());

			
			
			long paradigmID = piHomonym.m_lPradigmID;
			pHomonym->m_strSomeDescr = "";
			if( piHomonym.m_bOborot1 )
			{
				pHomonym->m_strSomeDescr += " <Ob1>";			
				int OborotId = piHomonym.m_OborotNo;
				assert (OborotId != -1);
				if (OborotId != -1)
				{
					CString str;
					str.Format(_T("Ob: %s"), FromRMLEncode(piHomonym.GetOborotPtr()->m_OborotEntryStr));
					pHomonym->m_strOborotsNum += str; 
				};
			}

			if (piHomonym.IsOb1()) {
				pHomonym->m_strSomeDescr += " <InOb>";
			}

			if( piHomonym.IsOb2() )
					pHomonym->m_strSomeDescr += " <Ob2>";

			
			if( piHomonym.m_SimplePrepNos.size() > 0 )
			{
				pHomonym->m_strSomeDescr = " (All preps:";			
				for(int k = 0 ; k < piHomonym.m_SimplePrepNos.size(); k++ )
				{
					CString str = ": ";
					str += FromRMLEncode(piSentence.GetOpt()->GetOborDic()->m_Entries[k].m_OborotEntryStr);
					pHomonym->m_strSomeDescr += str; 
				}
				pHomonym->m_strSomeDescr += ")"; 
			}



			pHomonym->m_iPradigmID = paradigmID;

			m_arrHomonyms.Add(pHomonym);
		}
		
		if( m_arrHomonyms.GetSize() > 1)
			m_bBold = TRUE;
		else
			m_bBold = FALSE;

		m_MainVerbs.clear();
		for (auto& w: piWord.m_MainVerbs)
			m_MainVerbs.push_back(w);

	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

int CVisualWord::GetWordLen(CDC* pDC)
{
	CSize sizeWord;
	sizeWord = pDC->GetOutputTextExtent(m_strWord);
	return sizeWord.cx;
}

int CVisualWord::GetWordHight(CDC* pDC)
{
	CSize sizeWord;
	sizeWord = pDC->GetOutputTextExtent(m_strWord);
	return sizeWord.cy;
}





BOOL CVisualWord::PrintWord(CDC* pDC, int iOffset)
{
	CFont* pOldFont = NULL;	


	COLORREF old_color;

	if( m_bInTermin )
	{			
		old_color = pDC->SetTextColor(RGB(0,0,255));
	}	
	else
	if( m_bArtificialCreated )
	{			
		old_color = pDC->SetTextColor(RGB(255,0,0));
	}	

	BOOL bSubjOrPredk = ((CVisualHomonym*)m_arrHomonyms.GetAt(m_iActiveHomonym))->m_bSubj || ((CVisualHomonym*)m_arrHomonyms.GetAt(m_iActiveHomonym))->m_bPredk;
	if( bSubjOrPredk )
	{
		pOldFont = pDC->GetCurrentFont(); 
		if( m_bBold ||  m_bArtificialCreated )
			pDC->SelectObject(& (CVisualSynanView::m_BoldUnderlineFontForWords) );
		else
			pDC->SelectObject(& (CVisualSynanView::m_UnderlineFontForWords) );

	}
	else
		if( m_bBold ||  m_bArtificialCreated )
		{
			pOldFont = pDC->GetCurrentFont(); 
			pDC->SelectObject(& (CVisualSynanView::m_BoldFontForWords) );
		};

	if( ((CVisualHomonym*)m_arrHomonyms.GetAt(m_iActiveHomonym))->m_bPredk )
	{
		CSize sz;
		pDC->MoveTo(m_WordRect.left, m_WordRect.bottom + 2 - iOffset);
		pDC->LineTo(m_WordRect.right, m_WordRect.bottom + 2 - iOffset);

	}
		

	pDC->TextOut(m_WordRect.left, m_WordRect.top - iOffset, m_strWord,m_strWord.GetLength());

	if( pOldFont )
		pDC->SelectObject(pOldFont);

	if( m_bInTermin || m_bArtificialCreated )
		pDC->SetTextColor(old_color);


	return TRUE;	
} 

int CVisualWord::CalculateCoordinates(CDC* pDC,int iX, int iY)
{
	CSize sizeWord;

	CSize rectWithBoldFont;
	CFont* pOldFont;

	if( m_arrHomonyms.GetSize() > 1 )
	{

		pOldFont = pDC->GetCurrentFont(); 
		pDC->SelectObject(&CVisualSynanView::m_BoldFontForWords);

		rectWithBoldFont = pDC->GetOutputTextExtent(m_strWord);
		pDC->SelectObject(pOldFont);
		sizeWord = pDC->GetOutputTextExtent(m_strWord);
		sizeWord.cx = rectWithBoldFont.cx;
		pDC->SelectObject(pOldFont);
	}
	else
		sizeWord = pDC->GetOutputTextExtent(m_strWord);
	
	m_WordRect.SetRect(iX,iY,sizeWord.cx + iX, sizeWord.cy + iY);

	return iX + sizeWord.cx;
}

int CVisualWord::PrintWord(CDC* pDC,int iX, int iY)
{
	CSize sizeWord;

	CFont* pOldFont;
	if( m_arrHomonyms.GetSize() > 1)
	{
		pOldFont = pDC->GetCurrentFont(); 
		pDC->SelectObject(& (CVisualSynanView::m_BoldFontForWords) );
	}

	pDC->TextOut(iX,iY,m_strWord,m_strWord.GetLength());

	CSize rectWithBoldFont;
	if( m_arrHomonyms.GetSize() > 1 )
	{
		rectWithBoldFont = pDC->GetOutputTextExtent(m_strWord);
		pDC->SelectObject(pOldFont);
		sizeWord = pDC->GetOutputTextExtent(m_strWord);
		sizeWord.cx = rectWithBoldFont.cx;
	}
	else
		sizeWord = pDC->GetOutputTextExtent(m_strWord);

	m_WordRect.SetRect(iX,iY,sizeWord.cx + iX, sizeWord.cy + iY);


	return iX + sizeWord.cx;
}


BOOL CVisualWord::PointInWordRect(CPoint& point)
{
	return m_WordRect.PtInRect(point);
}


void CVisualWord::GetHomonymsArray(CPtrArray** pHomonymsArray)
{
	if(pHomonymsArray != NULL) 
		(*pHomonymsArray) = &m_arrHomonyms;
}

BOOL CVisualWord::SetActiveHomonym(int iActiveHomonim)
{
	if( (iActiveHomonim < 0) || (iActiveHomonim >= m_arrHomonyms.GetSize()) )
		return FALSE;

	if(m_iActiveHomonym == iActiveHomonim)
		return FALSE;
	
	m_iActiveHomonym = iActiveHomonim;
	return TRUE;
}


void CVisualWord::ResetSubjAndPred()
{
	for(int i = 0 ; i < m_arrHomonyms.GetSize() ; i++ )
	{
		((CVisualHomonym*)m_arrHomonyms.GetAt(i))->m_bSubj = FALSE;
		((CVisualHomonym*)m_arrHomonyms.GetAt(i))->m_bPredk = FALSE;
	}

}

BOOL CVisualWord::GetActiveHomDescr(CString& strLemma,CString& strGramChar)
{
	strLemma = ((CVisualHomonym*)m_arrHomonyms.GetAt(m_iActiveHomonym))->m_strLemma; 
	
	strGramChar = m_strActiveGrammems;
	strGramChar = ((CVisualHomonym*)m_arrHomonyms.GetAt(m_iActiveHomonym))->m_strCommonGrammems + " "+strGramChar;
	strGramChar = ((CVisualHomonym*)m_arrHomonyms.GetAt(m_iActiveHomonym))->m_strPOS + " " + strGramChar;
	int ii = ((CVisualHomonym*)m_arrHomonyms.GetAt(m_iActiveHomonym))->m_iPradigmID;
	CString ss;
	ss.Format(_T("%d"), ii);
	strGramChar = strGramChar + " " + ss;
	strGramChar += ((CVisualHomonym*)m_arrHomonyms.GetAt(m_iActiveHomonym))->m_strSomeDescr;

	ss.Empty();
	ss.Format(_T(" (ClauseNo %d)"), m_iClauseNo);
	strGramChar += ss;

	strGramChar += m_strSomeDescr;
	return TRUE;
}




