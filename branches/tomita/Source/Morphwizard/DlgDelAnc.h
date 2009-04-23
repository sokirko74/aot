#if !defined(AFX_DLGDELANC_H__393EFE15_AE3F_11D4_B3CF_0050DA50CF03__INCLUDED_)
#define AFX_DLGDELANC_H__393EFE15_AE3F_11D4_B3CF_0050DA50CF03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDelAnc.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgDelAnc dialog

class CDlgDelAnc : public CDialog
{
// Construction
public:
	BOOL m_grm;
	CDlgDelAnc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDelAnc)
	enum { IDD = IDD_TOOL_DELANC };
	CString	m_file;
	CString	m_ancode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDelAnc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDelAnc)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDELANC_H__393EFE15_AE3F_11D4_B3CF_0050DA50CF03__INCLUDED_)
