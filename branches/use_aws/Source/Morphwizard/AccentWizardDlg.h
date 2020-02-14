#pragma once

#include "FileEditCtrl.h"
#include "smart_ptr.h"
#include "afxcmn.h"
#include "MorphwizardView.h"
#include <afxtempl.h>

// CAccentWizardDlg dialog

//----------------------------------------------------------------------------
class CAccentWizardDlg : public CDialog
{
	DECLARE_DYNAMIC(CAccentWizardDlg)

public:
	CAccentWizardDlg( CMorphwizardView& view, CWnd* pParent = NULL );   // standard constructor
	virtual ~CAccentWizardDlg();

// Dialog Data
	enum { IDD = IDD_ACCENTWIZARDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedOk();
	

	DECLARE_MESSAGE_MAP()

	bool	LoadFreqDict();
	void	_FillLemmaList();
	bool	FillLemmaList();
	bool	EditSelectedWord();
	MorphoWizard&	GetWizard()			{ return *m_view.GetWizard(); }
	
private:
	CMorphwizardView&	m_view;
	std::vector<lemma_iterator_t> m_lemmas;
	bool			m_hasListCtrl;
	int m_minFreq;
	CListCtrl m_listCtrl;
	BOOL m_bListShow;
	UINT m_allNonaccParaCount;
	UINT m_partialAccParaCount;
public:
	virtual BOOL OnInitDialog();

	BOOL m_allNonaccentedPara;
};
