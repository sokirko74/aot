// VisualSynanView.h : interface of the CVisualSynanView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALSYNANVIEW_H__1C505B41_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
#define AFX_VISUALSYNANVIEW_H__1C505B41_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VisualSynanDoc.h"

class CVisualSynanView : public CScrollView
{
protected: // create from serialization only
	CVisualSynanView();
	DECLARE_DYNCREATE(CVisualSynanView)

// Attributes
public:
	CVisualSynanDoc* GetDocument();
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualSynanView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVisualSynanView();


	BOOL   m_bExistUsefulFont;

	LOGFONT			m_LogFontForWords;
	static CFont	m_FontForWords;
	static CFont	m_FontForGroupNames;
	static CFont	m_BoldFontForWords;
	static CFont	m_BoldUnderlineFontForWords;
	static CFont	m_UnderlineFontForWords;
	

	void	UpdateFontsFromLogFont();

	
	void ResizeScroll();
	void Recalculate(CDC& clDC, CPrintInfo* pInfo = NULL);
	void Reset();
	void OnBuildRels(CString& str);
	void LogFontCpy(LOGFONT* dstFont, LOGFONT srcFont);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CPtrArray* m_pHomonymsArray;
	int m_iActiveWord;
	int m_iActiveSentence;

	//for tooltip
	int m_iActiveWordTT;
	int m_iActiveSentenceTT;

	BOOL   m_bDefaultFont;
	CToolTipCtrl m_ctrlToolTip;
	BOOL m_bShowGroups;
	int  m_iStartSent;
	int	 m_iStartLine;
	int  m_iOffset;
	BOOL m_bFirsTime;
	BOOL m_bMore;

	

	
// Generated message  functions
protected:
	int OnNeedText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CVisualSynanView)
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFonts();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSynHelp();
	afx_msg void OnViewTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in VisualSynanView.cpp
inline CVisualSynanDoc* CVisualSynanView::GetDocument()
   { return (CVisualSynanDoc*)m_pDocument; }
#endif

int CALLBACK TestIfTrueTypeEx(
  const LOGFONT FAR *lpelf,  // pointer to logical-font data
  const TEXTMETRIC FAR *lpntm,  // pointer to physical-font data
  unsigned long FontType,            // type of font
  LPARAM lParam            ) ;// pointer to application-defined data);

int CALLBACK GetFefaultFontEx(
  const LOGFONT FAR *lpelf,  // pointer to logical-font data
  const TEXTMETRIC FAR *lpntm,  // pointer to physical-font data
  unsigned long FontType,            // type of font
  LPARAM lParam            ); // pointer to application-defined data);


 




/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALSYNANVIEW_H__1C505B41_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
