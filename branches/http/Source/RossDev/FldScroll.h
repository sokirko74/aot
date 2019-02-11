#if !defined(AFX_FLDSCROLL_H__E327EEC6_CD51_11D2_A6EE_F4DEBC000000__INCLUDED_)
#define AFX_FLDSCROLL_H__E327EEC6_CD51_11D2_A6EE_F4DEBC000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FldScroll.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFldScroll window

class CFldScroll : public CScrollBar
{
// Construction
public:
	CFldScroll();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFldScroll)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFldScroll();

	// Generated message  functions
protected:
	//{{AFX_MSG(CFldScroll)
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLDSCROLL_H__E327EEC6_CD51_11D2_A6EE_F4DEBC000000__INCLUDED_)
