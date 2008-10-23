#if !defined(AFX_POSCHOICER_H__591DF6D6_14F5_11D3_A775_8284D9000000__INCLUDED_)
#define AFX_POSCHOICER_H__591DF6D6_14F5_11D3_A775_8284D9000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PosChoicer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPosChoicer dialog

class CPosChoicer : public CPropertyPage
{
  DECLARE_DYNCREATE(CPosChoicer)
// Construction
public:
	CPosChoicer();   // standard constructor
	vector<CFormInfo>	m_ParadigmCollection;
	~CPosChoicer()
	{
		
	}
	
	CString m_ResultString;

// Dialog Data
	//{{AFX_DATA(CPosChoicer)
     enum { IDD = IDD_POS_CHOICER };
	 CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPosChoicer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CPosChoicer)
	virtual BOOL OnInitDialog();
	virtual LRESULT OnWizardNext();
     virtual BOOL OnSetActive();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSCHOICER_H__591DF6D6_14F5_11D3_A775_8284D9000000__INCLUDED_)
