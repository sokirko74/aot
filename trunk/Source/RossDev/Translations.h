#if !defined(AFX_TRANSLATIONS_H__56B9C2A6_39C0_11D3_A7B7_460A1C000000__INCLUDED_)
#define AFX_TRANSLATIONS_H__56B9C2A6_39C0_11D3_A7B7_460A1C000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CTranslations.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTranslations dialog

struct  TransLemma {
	long ParadigmId;
	long PartOfSpeech;
	TransLemma(long _ParadigmId, long _PartOfSpeech)
	{
		ParadigmId = _ParadigmId;
		PartOfSpeech = _PartOfSpeech;
	};
};

class CTranslations : public CPropertyPage
{
	DECLARE_DYNCREATE(CTranslations)

// Construction
public:

	CTranslations();
	vector<CFormInfo> m_ParadigmCollection;
	vector<TransLemma> RusLemmas;
	CString m_ResultString;
	

// Dialog Data
	//{{AFX_DATA(CTranslations)
	enum { IDD = IDD_ENGLISH_EQU };
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTranslations)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message  functions
	//{{AFX_MSG(CTranslations)
	virtual BOOL OnInitDialog();
    virtual BOOL OnWizardFinish( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSLATIONS_H__56B9C2A6_39C0_11D3_A7B7_460A1C000000__INCLUDED_)
