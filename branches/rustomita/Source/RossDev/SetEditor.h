#if !defined(AFX_SETEDITOR_H__1B51B373_D5F5_11D2_A6FB_38A1B7000000__INCLUDED_)
#define AFX_SETEDITOR_H__1B51B373_D5F5_11D2_A6FB_38A1B7000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetEditor dialog

class CSetEditor : public CDialog
{
// Construction
public:
	CSetEditor(CRossDoc*		pRossDoc,
		       vector<TCortege>& OldValue,
		       vector<TCortege>& NewValue,
			   CRossDevTextField& F,
		       CWnd* pParent = NULL);   // standard constructor

	vector<TCortege>& m_OldValue;
	vector<TCortege>& m_NewValue;
	CRossDevTextField& m_Field;
    CRossDoc* m_pRossDoc;
    CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}



// Dialog Data
	//{{AFX_DATA(CSetEditor)
	enum { IDD = IDD_SETEDITOR };
	CListBox	m_ListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CSetEditor)
	virtual BOOL OnInitDialog();
	afx_msg void OnSeteditorDel();
	afx_msg void OnSeteditorAdd();
	afx_msg void OnSeteditorValue();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETEDITOR_H__1B51B373_D5F5_11D2_A6FB_38A1B7000000__INCLUDED_)
