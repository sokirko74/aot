#if !defined(AFX_NEWTEXTENTRY_H__26E553A0_39EE_11D3_A7B8_6CE586000000__INCLUDED_)
#define AFX_NEWTEXTENTRY_H__26E553A0_39EE_11D3_A7B8_6CE586000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewTextEntry.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewTextEntry dialog


class CNewTextEntry : public CPropertyPage
{
// Construction
public:
	const CDictionary* m_pRoss;
	const CDictionary* GetRoss () {return m_pRoss;}

	CNewTextEntry(CWnd* pParent = NULL);   // standard constructor
	

// Dialog Data
	//{{AFX_DATA(CNewTextEntry)
	enum { IDD = IDD_NEW_TEXTENTRY };
	CString	m_UnitStr;
	CString	m_MeanNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTextEntry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
     virtual BOOL    OnSetActive();
     virtual LRESULT OnWizardNext();

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CNewTextEntry)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTEXTENTRY_H__26E553A0_39EE_11D3_A7B8_6CE586000000__INCLUDED_)
