// VisualSynanDoc.h : interface of the CVisualSynanDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALSYNANDOC_H__1C505B3F_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
#define AFX_VISUALSYNANDOC_H__1C505B3F_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "VisualSentences.h"

class CVisualSynanDoc : public CDocument
{
protected: // create from serialization only
	CVisualSynanDoc();
	DECLARE_DYNCREATE(CVisualSynanDoc)

// Attributes
public:
	CVisualSentences m_VisualSentences;
	CString			 m_WorkTimeStr;

// Operations
public:

	BOOL ProcessString(CString strText);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualSynanDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void PreCloseFrame( CFrameWnd* pFrame );
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL



// Implementation
public:
	virtual ~CVisualSynanDoc();

	void BuildRels(CString& str)
	{
		m_VisualSentences.BuildRels(str);
	}

	BOOL GetHomonymsArray(CPoint& point,CPtrArray** pHomonymsArray, int* iActiveSentence,int* iActiveWord)
	{
		return m_VisualSentences.GetHomonymsArray(point,pHomonymsArray, iActiveSentence,iActiveWord);
	}

	BOOL SetActiveHomonym(int iActiveSentence,int iActiveWord, int iActiveHomonim)
	{
		return m_VisualSentences.SetActiveHomonym(iActiveSentence,iActiveWord, iActiveHomonim);
	}

	CSize GetSentencesSize()
	{
		return m_VisualSentences.GetSentencesSize();
	}

	BOOL GetActiveHomDescr(int m_iActiveSentenceTT,int m_iActiveWordTT,CString& strLemma,CString& strGramChar)
	{
		return m_VisualSentences.GetActiveHomDescr(m_iActiveSentenceTT,m_iActiveWordTT,strLemma,strGramChar);
	}

	BOOL CalculateCoordinates(CDC* pDC, int iWidth, BOOL bShowGroups)
	{
		return m_VisualSentences.CalculateCoordinates(pDC, iWidth, bShowGroups);
	}

	BOOL PrintSentences(CDC* pDC,CRect& rectForDrawing, int iOffset)
	{
		return m_VisualSentences.PrintSentences(pDC,rectForDrawing, iOffset);
	}

	BOOL NoSentences()
	{
		return (m_VisualSentences.SentCount() == 0);
	}
	int PrintSentencesOnPrn(CDC* pDC, int iSent, int& iLine, int iBottom, int& iOffset)
	{
		return m_VisualSentences.PrintSentencesOnPrn(pDC, iSent, iLine, iBottom, iOffset);
	}
	void Reset()
	{
		m_VisualSentences.Reset();
	}


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:	
// Generated message  functions
protected:
	//{{AFX_MSG(CVisualSynanDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALSYNANDOC_H__1C505B3F_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
