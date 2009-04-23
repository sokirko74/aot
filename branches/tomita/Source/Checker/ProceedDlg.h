// ProceedDld.h : header file
#if !defined(AFX_PROCEEDDLG_H__506BF523_3AE1_11D5_92A5_005004035BFC__INCLUDED_)
#define AFX_PROCEEDDLG_H__506BF523_3AE1_11D5_92A5_005004035BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

#include "../common/utilit.h"

/////////////////////////////////////////////////////////////////////////////
// CProceedDlg dialog

class CProceedDlg : public CDialog
{
// Construction
public:
	CProceedDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProceedDlg)
	enum { IDD = IDD_PROCEED_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Data
public:
	int m_CurrentBadSentence;

	int m_BaseCurrentNo;
	int m_BaseLastNo;
	int m_BaseStartNo;
	

	void SetBaseVariant(int no);
	void ShowDifferences();
	void UpdateOldBaseOnTheScreen();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProceedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CProceedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnSkip();
	afx_msg void OnNext();
	afx_msg void OnPrev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddAll();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCEEDDLG_H__506BF523_3AE1_11D5_92A5_005004035BFC__INCLUDED_)
