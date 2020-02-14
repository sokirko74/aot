#pragma once

#include "SizeFormView.h"
#include "../MorphWizardLib/wizard.h"
class CSLFDocument;

// CParadigmDifferences form view

class CParadigmDifferences : public CSizeFormView
{
	DECLARE_DYNCREATE(CParadigmDifferences)

protected:
	CParadigmDifferences();           // protected constructor used by dynamic creation
	virtual ~CParadigmDifferences();

public:
	enum { IDD = IDD_PARAD_DIFF };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	MorphoWizard* GetWizard();
	CSLFDocument* GetDocument();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_Results;
	int m_Paradigm1;
	int m_Paradigm2;
	afx_msg void OnBnClickedAnalyse();

	afx_msg void OnBnClickedCancel();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};


