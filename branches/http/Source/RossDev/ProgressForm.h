#if !defined(AFX_PROGRESSFORM_H__4E7B628B_157D_4035_9FF1_47853C5C5C53__INCLUDED_)
#define AFX_PROGRESSFORM_H__4E7B628B_157D_4035_9FF1_47853C5C5C53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressForm dialog
class CSemanticStrView;
class CProgressForm : public CDialog
{
// Construction
public:
	long m_CurrentPos;
	long m_Max; 
	CProgressForm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressForm)
	enum { IDD = IDD_PROGRESSBAR };
	CProgressCtrl	m_ProgressCtrl;
	CString	m_Statis;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CProgressForm)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSFORM_H__4E7B628B_157D_4035_9FF1_47853C5C5C53__INCLUDED_)
