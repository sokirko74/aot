#if !defined(AFX_DLGHOM_H__724C95D3_2CC4_11D5_929B_005004035BFC__INCLUDED_)
#define AFX_DLGHOM_H__724C95D3_2CC4_11D5_929B_005004035BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgHom.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgHom dialog

class CDlgHom : public CDialog
{
// Construction
public:
	CDlgHom(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgHom)
	enum { IDD = IDD_DLG_HOM };
	int		m_type;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHom)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CDlgHom)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHOM_H__724C95D3_2CC4_11D5_929B_005004035BFC__INCLUDED_)
