// AccentWizardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Morphwizard.h"
#include "AccentWizardDlg.h"
#include "SLFDocument.h"
#include "ProgressBar.h"

//////////////////////////////////////////////////////////////////////////////
// CAccentWizardDlg class implementation
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CAccentWizardDlg, CDialog)
CAccentWizardDlg::CAccentWizardDlg( CMorphwizardView& view, CWnd* pParent /*=NULL*/)
	: CDialog(CAccentWizardDlg::IDD, pParent), m_view(view)
	, m_minFreq(10)
	, m_bListShow(FALSE)
	, m_allNonaccParaCount(0)
	, m_partialAccParaCount(0)
	, m_allNonaccentedPara(FALSE)
{
}

CAccentWizardDlg::~CAccentWizardDlg()
{
}

void CAccentWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//	DDV_FileEditCtrl(pDX, IDC_FREQ_DICT_EDIT);
	DDX_Text(pDX, IDC_MAX_COUNT_EDIT, m_minFreq);
	DDX_Control(pDX, IDC_LEMMA_LIST, m_listCtrl);
	DDX_Check(pDX, IDC_LIST_SHOW_CHECK, m_bListShow);
	DDX_Text(pDX, IDC_EDIT_ALL_NONACCENTED_PARA, m_allNonaccParaCount);
	DDX_Text(pDX, IDC_EDIT_PARTIAL_ACCENTED_PARA, m_partialAccParaCount);
	DDX_Radio(pDX, IDC_RADIO_PARTIAL_ACCENTED_PARA, m_allNonaccentedPara);
}


BEGIN_MESSAGE_MAP(CAccentWizardDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAccentWizardDlg message handlers

void CAccentWizardDlg::OnBnClickedOk()
{
	OnOK();
}


BOOL CAccentWizardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWizardProgressMeter meter(GetWizard());
	for( lemma_iterator_t it=GetWizard().m_LemmaToParadigm.begin(); 
			it!=GetWizard().m_LemmaToParadigm.end(); it++ ) 
	{
		if( GetWizard().HasUnknownAccents(it) ) 
		{
			++m_allNonaccParaCount;
			if( GetWizard().IsPartialAccented(it) ) 
			++m_partialAccParaCount;
		}
	}

	if( m_partialAccParaCount==0 ) 
	{
		GetDlgItem(IDC_RADIO_PARTIAL_ACCENTED_PARA)->EnableWindow(FALSE);
		m_allNonaccentedPara = 1;
		if( m_allNonaccParaCount==0 ) 
		{
			GetDlgItem(IDC_RADIO_ALL_NONACCENTED_PAPA)->EnableWindow(FALSE);
			GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		}
	}

	

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


