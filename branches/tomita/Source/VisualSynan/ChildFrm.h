// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__1C505B3D_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
#define AFX_CHILDFRM_H__1C505B3D_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const char g_strFontNameRegPath[] = "Software\\Dialing\\VisualSynan\\TextFontName";
const char g_strFontSizeRegPath[] = "Software\\Dialing\\VisualSynan\\TextFontSize";


class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message  functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRunSyntax();
	afx_msg void OnGermanSyntax();
	afx_msg void OnRussianSyntax();
	afx_msg void OnUpdateRunSyntax(CCmdUI* pCmdUI);
	afx_msg void OnBuildRels();
	afx_msg void OnKillHom();
	afx_msg void OnShowMessages();

	//}}AFX_MSG

	BOOL m_bFirst; 
	CSplitterWnd m_wndSplitter;
	CVisualSynanView* m_pSynView;
	CEditView* m_pEditView;
	BOOL m_bNewDoc;
	CFont m_Font;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileShowgraphematicsresults();
	afx_msg void OnFileShowmorphologyresults();
	afx_msg void OnFileShowpostmorph();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__1C505B3D_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
