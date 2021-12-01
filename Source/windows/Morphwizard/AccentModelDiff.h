#pragma once

#include "morph_dict/MorphWizardLib/wizard.h"
class CSLFDocument;

class CAccentModelDiff : public CFormView
{
	DECLARE_DYNCREATE(CAccentModelDiff)

protected:
	CAccentModelDiff();           // protected constructor used by dynamic creation
	virtual ~CAccentModelDiff();

public:
	enum { IDD = IDD_ACCENT_MODEL_DIFF };
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
	int m_AccentModel1;
	int m_AccentModel2;
	afx_msg void OnBnClickedAnalyse();

	afx_msg void OnBnClickedCancel();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};


