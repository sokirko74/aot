#if !defined(AFX_FINDBYSTRDLG_H__38BA6D2B_3B0B_4D6C_9722_93ED12F68B97__INCLUDED_)
#define AFX_FINDBYSTRDLG_H__38BA6D2B_3B0B_4D6C_9722_93ED12F68B97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindByStrDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindByStrDlg dialog
class CRossDoc;
class CFindByStrDlg : public CDialog
{
// Construction
public:
	CFindByStrDlg(CWnd* pParent, CRossDoc* ActiveRossDoc);   // standard constructor
	CRossDoc* m_pActiveRossDoc;

// Dialog Data
	//{{AFX_DATA(CFindByStrDlg)
	enum { IDD = IDD_FIND_BY_STR_DLG };
	int		m_FindMode;
	CString	m_FindString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindByStrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CFindByStrDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDBYSTRDLG_H__38BA6D2B_3B0B_4D6C_9722_93ED12F68B97__INCLUDED_)
