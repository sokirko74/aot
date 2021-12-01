#include "afxcmn.h"
#if !defined(AFX_FIELDEDITOR_H__5E606C65_E428_11D2_A723_0E3828000000__INCLUDED_)
#define AFX_FIELDEDITOR_H__5E606C65_E428_11D2_A723_0E3828000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FieldEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFieldEditor dialog

class CFieldEditor : public CDialog
{
// Construction
	BOOL InitItem(int ItemNo);
public:
	CRossDoc* m_pRossDoc;
	CDictionary* GetRoss () {return m_pRossDoc->GetRoss();};
	BYTE    m_FieldNo;
	int		m_frOne;
	int		m_frMany;
	int		m_frFormula;

	CFieldEditor(CRossDoc* pRossDoc,
				 BYTE FieldNo,
				 CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFieldEditor)
	enum { IDD = IDD_FIELDEDITOR_FORM };
	CString	m_FieldStr;
	BOOL	m_bApplicToActant;
	CString	m_OrderIdStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CFieldEditor)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio2();
	afx_msg void OnRadio1();
	afx_msg void OnRadio3();
	virtual void OnOK();
	afx_msg void OnEditSignat();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	bool AddNewSignat();
public:
	CListCtrl m_Signats;
	afx_msg void OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDEDITOR_H__5E606C65_E428_11D2_A723_0E3828000000__INCLUDED_)
