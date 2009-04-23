#if !defined(AFX_NEWREFARTICLES_H__42CE6AB4_6436_11D3_A7FC_66CDA8000000__INCLUDED_)
#define AFX_NEWREFARTICLES_H__42CE6AB4_6436_11D3_A7FC_66CDA8000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewRefArticles.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewRefArticles dialog

class CNewRefArticles : public CPropertyPage
{
	DECLARE_DYNCREATE(CNewRefArticles)

// Construction
public:
	vector<LONG>   m_RefsId;
	CString        m_ResultString;
	CString        m_Caption;
	CString        m_Lemma;

	CNewRefArticles();
	CNewRefArticles(CString Caption);
	~CNewRefArticles();
	void SetRefs(vector<LONG>& RefsId);


// Dialog Data
	//{{AFX_DATA(CNewRefArticles)
	enum { IDD = IDD_REF_ARTICLES };
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNewRefArticles)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message  functions
	//{{AFX_MSG(CNewRefArticles)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWREFARTICLES_H__42CE6AB4_6436_11D3_A7FC_66CDA8000000__INCLUDED_)
