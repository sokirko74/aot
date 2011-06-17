#if !defined(AFX_NEWARTICLEWIZARD_H__26E55396_39EE_11D3_A7B8_6CE586000000__INCLUDED_)
#define AFX_NEWARTICLEWIZARD_H__26E55396_39EE_11D3_A7B8_6CE586000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewArticleWizard.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CNewArticleWizard

class CNewArticleWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CNewArticleWizard)

// Construction
public:
	CNewArticleWizard(LPCTSTR pszCaption, CRossDoc* pRossDoc);

// Attributes
public:
	LONG PartOfSpeech;
	CRossDoc* m_pRossDoc;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewArticleWizard)
	public:
	virtual BOOL OnSetActive( );
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNewArticleWizard();

	// Generated message  functions
protected:
	//{{AFX_MSG(CNewArticleWizard)
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWARTICLEWIZARD_H__26E55396_39EE_11D3_A7B8_6CE586000000__INCLUDED_)
