#if !defined(AFX_MYLISTBOX_H__7AFDD534_17AA_11D4_AE7A_00104B07B1E3__INCLUDED_)
#define AFX_MYLISTBOX_H__7AFDD534_17AA_11D4_AE7A_00104B07B1E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ttlistbox.h : header file
//

#include "tooltip2.h"


/////////////////////////////////////////////////////////////////////////////
// CTTListBox window

class CTTListBox : public CListCtrl
{
// Construction
public:
	CTTListBox();

// Attributes
public:

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTTListBox)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	CString (*m_fnTTTextRetriever)( int);
	CToolTip2	m_ToolTip;
	CString		m_strToolTipText;
	int			m_nPrevItem;			// item over which the mouse cursor was
	BOOL		m_bMouseOver;

	void SetToolTipText(LPCTSTR string);
	LRESULT OnMouseHover (WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void ShowToolTip();
	//{{AFX_MSG(CTTListBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTBOX_H__7AFDD534_17AA_11D4_AE7A_00104B07B1E3__INCLUDED_)
