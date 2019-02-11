#if !defined(AFX_WAITDLG_H__12BCEFE3_680B_11D2_B1BC_82C397093556__INCLUDED_)
#define AFX_WAITDLG_H__12BCEFE3_680B_11D2_B1BC_82C397093556__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WaitDlg.h : header file
//

//#include "thread.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CWaitAsyncDialog dialog

class CWaitAsyncDialog : public CDialog
{
// Construction
public:
	CWaitAsyncDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitAsyncDialog(); 
	BOOL Create();
// Dialog Data
	//{{AFX_DATA(CWaitAsyncDialog)
	enum { IDD = IDD_WAIT_DLG };
	CAnimateCtrl	m_WaitAnimation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitAsyncDialog)
	virtual BOOL OnInitDialog( );
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message  functions
	//{{AFX_MSG(CWaitAsyncDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAITDLG_H__12BCEFE3_680B_11D2_B1BC_82C397093556__INCLUDED_)
