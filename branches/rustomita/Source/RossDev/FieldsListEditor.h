#if !defined(AFX_FIELDSLISTEDITOR_H__5E606C64_E428_11D2_A723_0E3828000000__INCLUDED_)
#define AFX_FIELDSLISTEDITOR_H__5E606C64_E428_11D2_A723_0E3828000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FieldsListEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFieldsListEditor dialog

class CFieldListCtrl : public CListCtrl 	
{
};

class CFieldsListEditor : public CDialog
{
// Construction
public:
	CRossDoc* m_pRossDoc;
	CDictionary* GetRoss () {return m_pRossDoc->GetRoss();};

	CFieldsListEditor(CRossDoc* pRossDoc, CWnd* pParent =NULL);
	void Update();


// Dialog Data
	//{{AFX_DATA(CFieldsListEditor)
	enum { IDD = IDD_FIELDS_EDITOR_FORM };
	CListBox	m_FieldList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldsListEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CFieldsListEditor)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnAddButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDSLISTEDITOR_H__5E606C64_E428_11D2_A723_0E3828000000__INCLUDED_)
