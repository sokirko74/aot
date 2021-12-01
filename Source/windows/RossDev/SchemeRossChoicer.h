#if !defined(AFX_SCHEMEROSSCHOICER_H__5E606C63_E428_11D2_A723_0E3828000000__INCLUDED_)
#define AFX_SCHEMEROSSCHOICER_H__5E606C63_E428_11D2_A723_0E3828000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SchemeRossChoicer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SchemeRossChoicer dialog

class SchemeRossChoicer : public CDialog
{
// Construction
public:
    CRossDoc* m_pRossDoc;
	CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}
	
	SchemeRossChoicer(CRossDoc* pRossDoc,
		         CWnd* pParent = NULL);   // standard constructor

	

// Dialog Data
	//{{AFX_DATA(SchemeRossChoicer)
	enum { IDD = IDD_SCHEME_CHOICER };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SchemeRossChoicer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(SchemeRossChoicer)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	virtual void OnOK();
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEMEROSSCHOICER_H__5E606C63_E428_11D2_A723_0E3828000000__INCLUDED_)
