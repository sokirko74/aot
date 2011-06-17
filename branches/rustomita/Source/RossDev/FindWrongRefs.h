#if !defined(AFX_FINDWRONGREFS_H__5D3234FD_6872_4149_9FA0_441F75FD05F7__INCLUDED_)
#define AFX_FINDWRONGREFS_H__5D3234FD_6872_4149_9FA0_441F75FD05F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindWrongRefs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindWrongRefs dialog
class CRossDoc;
class CFindWrongRefs : public CDialog
{
// Construction
public:
	CRossDoc* m_pActiveRossDoc;
	CFindWrongRefs(CWnd* pParent, CRossDoc* ActiveRossDoc);   
	

// Dialog Data
	//{{AFX_DATA(CFindWrongRefs)
	enum { IDD = IDD_FIND_WRONG_REFS };
	CString	m_FieldStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindWrongRefs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CFindWrongRefs)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDWRONGREFS_H__5D3234FD_6872_4149_9FA0_441F75FD05F7__INCLUDED_)
