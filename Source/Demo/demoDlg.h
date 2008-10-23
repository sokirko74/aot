// demoDlg.h : header file
//

#if !defined(AFX_DEMODLG_H__59A50E5C_1A5C_11D3_9CCC_00105A68BAF2__INCLUDED_)
#define AFX_DEMODLG_H__59A50E5C_1A5C_11D3_9CCC_00105A68BAF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#import "../../bin/lemmatizer.tlb"
#import "../../bin/agramtab.tlb"

/////////////////////////////////////////////////////////////////////////////
// CDemoDlg dialog

class CDemoDlg : public CDialog
{
// Construction
public:
	CDemoDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CDemoDlg();	

// Dialog Data
	//{{AFX_DATA(CDemoDlg)
	enum { IDD = IDD_DEMO_DIALOG };
	CButton	m_buttonExit;
	CButton	m_buttonFind;
	CListCtrl	m_ctrlResults;
	CString	m_strSrc;
	int		m_iLang;
	int		m_iSubject;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl*					m_pToolTip;
	HICON	m_hIcon;
	LEMMATIZERLib::ILemmatizerPtr	m_piRussian;
	LEMMATIZERLib::ILemmatizerPtr	m_piEnglish;
	LEMMATIZERLib::ILemmatizerPtr	m_piGerman;
	AGRAMTABLib::IGramTabPtr		m_piRussianAgram;
	AGRAMTABLib::IGramTabPtr		m_piEnglishAgram;
	AGRAMTABLib::IGramTabPtr		m_piGermanAgram;
	bool m_bRussianLoaded; 
	bool m_bEnglishLoaded;
	bool m_bGermanLoaded;
	bool m_bEngAgramLoaded;	
	bool m_bRusAgramLoaded;	
	bool m_bGerAgramLoaded;	
	AGRAMTABLib::IGramTabPtr		GetGramTabByLanguage(int Lang);
	LEMMATIZERLib::ILemmatizerPtr	GetLemmatizerByLanguage(int Lang);
	const char* CDemoDlg::GetStrByLanguage(int Lang);
	// Generated message map functions
	//{{AFX_MSG(CDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonFind();
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRadioLiterature();
	afx_msg void OnRadioFinance();
	afx_msg void OnRadioComputer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMODLG_H__59A50E5C_1A5C_11D3_9CCC_00105A68BAF2__INCLUDED_)
