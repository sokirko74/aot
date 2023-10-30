#pragma once 
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

class CDemoDlg : public CDialog
{
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
	CMorphanHolder	m_piRussian;
	CMorphanHolder	m_piEnglish;
	CMorphanHolder	m_piGerman;
	CMorphanHolder& GetHolder(MorphLanguageEnum Lang);
	const CAgramtab* GetGramTabByLanguage(MorphLanguageEnum Lang);
	const CLemmatizer* GetLemmatizerByLanguage(MorphLanguageEnum Lang);
	bool BuildParadigmsForOneLanguage(MorphLanguageEnum langua, CString src_str, bool OnlyDigits);
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

