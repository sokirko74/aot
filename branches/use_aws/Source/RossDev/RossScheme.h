#if !defined(AFX_ROSSSCHEME_H__82A8EED7_DAED_11D2_A703_D63360000000__INCLUDED_)
#define AFX_ROSSSCHEME_H__82A8EED7_DAED_11D2_A703_D63360000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RossScheme.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRossScheme dialog

class CRossScheme : public CDialog
{
// Construction
public:
	CRossDoc*     m_pRossDoc;
	CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}
	
	CRossScheme(CRossDoc* pRossDoc,
		         CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRossScheme)
	enum { IDD = IDD_SCHEME };
	CListCtrl	m_ListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRossScheme)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CRossScheme)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditDomain();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSaveToFiles();
	afx_msg void OnBnClickedLoadConstFromFiles();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROSSSCHEME_H__82A8EED7_DAED_11D2_A703_D63360000000__INCLUDED_)
