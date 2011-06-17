#if !defined(AFX_SIGNATEDITOR_H__A3F0300E_ABEB_4E14_8462_0420DD39E5F8__INCLUDED_)
#define AFX_SIGNATEDITOR_H__A3F0300E_ABEB_4E14_8462_0420DD39E5F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SignatEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SignatEditor dialog

class CSignatEditor : public CDialog
{
// Construction
public:
	CString		m_DomainString;
	CRossDoc*     m_pRossDoc;
	CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}

	CSignatEditor(CString DomainString, CString Name, int OrderNo, CRossDoc* pRossDoc, CWnd* pParent = NULL);   

// Dialog Data
	//{{AFX_DATA(SignatEditor)
	enum { IDD = IDD_SIGNAT_EDITOR };
	CListBox	m_SignatDoms;
	CListBox	m_AvailDoms;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SignatEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(SignatEditor)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	DWORD m_OrderNo;
	CString m_Name;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGNATEDITOR_H__A3F0300E_ABEB_4E14_8462_0420DD39E5F8__INCLUDED_)
