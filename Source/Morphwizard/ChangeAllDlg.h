#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../MorphWizardLib/wizard.h"

// CChangeAllDlg dialog

class CChangeAllDlg : public CDialog
{
	DECLARE_DYNAMIC(CChangeAllDlg)

public:
	int m_anyAccentModels;
	int m_usedAccentModel;

	CChangeAllDlg( const MorphoWizard& wizard, const std::vector<lemma_iterator_t>& found_paradigms, 
		WORD oldAccentModelNo, WORD newAccentModelNo, CWnd* pParent = NULL);   // standard constructor

	virtual ~CChangeAllDlg();

// Dialog Data
	enum { IDD = IDD_CHANGE_ALL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedSameAccentRadio();
	afx_msg void OnBnClickedAnyAccentRadio();

	DECLARE_MESSAGE_MAP()
	CListCtrl m_list;
public:
	virtual BOOL OnInitDialog();
private:
	void	updateParadigmList();

	const MorphoWizard& m_wizard;
	const std::vector<lemma_iterator_t>& m_found_paradigms;

	int	m_newAccentModelNo;
	int	m_oldAccentModelNo;
};
