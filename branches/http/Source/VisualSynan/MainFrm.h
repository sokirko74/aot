// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__1C505B3B_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
#define AFX_MAINFRM_H__1C505B3B_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VisualSynanView.h"
#include "WaitThread.h"



class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	

// Attributes
public:
	CString GetCurFileName()
	{ return m_strCurFileName;}
	bool LoadSyntaxByLanguage(MorphLanguageEnum t);

// Operations
public:
	CString GetFileWithThisFilter(CString strFilter);
	BOOL m_bNewDoc;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	
	CString		m_strCurFileName;
	CWaitThread* m_pWaitThread;

// Generated message  functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBuildFromFile();
	afx_msg void OnReinitializeSyntax();
	
	afx_msg LRESULT OnProcessTxtFile(WPARAM, LPARAM);
	afx_msg void OnSynHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__1C505B3B_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
