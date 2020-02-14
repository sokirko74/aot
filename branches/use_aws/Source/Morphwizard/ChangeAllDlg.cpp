// ChangeAllDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Morphwizard.h"
#include "ChangeAllDlg.h"


// CChangeAllDlg dialog

IMPLEMENT_DYNAMIC(CChangeAllDlg, CDialog)

CChangeAllDlg::CChangeAllDlg( const MorphoWizard& wizard, const std::vector<lemma_iterator_t>& found_paradigms, 
			  WORD oldAccentModelNo, WORD newAccentModelNo, CWnd* pParent)
	: CDialog(CChangeAllDlg::IDD, pParent), m_wizard(wizard), m_found_paradigms(found_paradigms),
	m_oldAccentModelNo(oldAccentModelNo), m_newAccentModelNo(newAccentModelNo)
{
	m_usedAccentModel = 1;
	m_anyAccentModels = 0;
}

CChangeAllDlg::~CChangeAllDlg()
{
}

void CChangeAllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LEMMA_LIST, m_list);
	DDX_Radio(pDX, IDC_SAME_ACCENT_RADIO, m_anyAccentModels);
	DDX_Radio(pDX, IDC_RADIO_OLD_ACC, m_usedAccentModel);
}


BEGIN_MESSAGE_MAP(CChangeAllDlg, CDialog)
	ON_BN_CLICKED(IDC_SAME_ACCENT_RADIO, OnBnClickedSameAccentRadio)
	ON_BN_CLICKED(IDC_ANY_ACCENT_RADIO, OnBnClickedAnyAccentRadio)
END_MESSAGE_MAP()


// CChangeAllDlg message handlers

BOOL CChangeAllDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list.InsertColumn(0,"Model No");
	m_list.InsertColumn(1,"Paradigms");
	CRect r;
	m_list.GetWindowRect(&r);
	m_list.SetColumnWidth(0,r.Width()/4);
	m_list.SetColumnWidth(1,3*r.Width()/4);

	m_anyAccentModels = 0;
	int count=0;
	for( int i=0; i<m_found_paradigms.size(); ++i ) 
	{	
		if( m_found_paradigms[i]->second.m_AccentModelNo!= m_oldAccentModelNo )
		{
			m_anyAccentModels = 1;
			++count;
		}
	}

	if( m_anyAccentModels==0 || count==m_found_paradigms.size() ) 
	{
		GetDlgItem(IDC_SAME_ACCENT_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_ANY_ACCENT_RADIO)->EnableWindow(FALSE);
	}

	if( m_anyAccentModels==0 && m_oldAccentModelNo==m_newAccentModelNo ) 
	{
		m_usedAccentModel = 2;
		GetDlgItem(IDC_RADIO_OLD_ACC)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OLD_NEW_ACC)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_NEW_ACC)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);

	updateParadigmList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeAllDlg::updateParadigmList()
{
	CString s;
	int count=0;
	m_list.DeleteAllItems();
	for( int i=0; i<m_found_paradigms.size(); ++i ) 
	{
		if( m_anyAccentModels==0
			&& m_found_paradigms[i]->second.m_AccentModelNo!= m_oldAccentModelNo )
			continue;

		s.Format("%d (%d)",m_found_paradigms[i]->second.m_FlexiaModelNo,
				m_found_paradigms[i]->second.m_AccentModelNo);
		m_list.InsertItem(count,s);

		std::string Lemma = m_wizard.get_lemm_string(m_found_paradigms[i]);
		std::string PrefixSet = m_wizard.get_prefix_set( m_found_paradigms[i] );
		if (!PrefixSet.empty())
			Lemma = PrefixSet+'|'+Lemma;
		RmlMakeLower(Lemma, m_wizard.m_Language);
		m_list.SetItemText(count, 1, Lemma.c_str());

		++count;
	}

	s.Format("Paradigms: %d",count);
	LVCOLUMN col;
	col.cchTextMax = s.GetLength();
	col.pszText = s.GetBuffer(s.GetLength());
	col.mask = LVCF_TEXT;
	m_list.SetColumn(1,&col);
	s.ReleaseBuffer();
}

void CChangeAllDlg::OnBnClickedSameAccentRadio()
{
	m_anyAccentModels = 0;
	updateParadigmList();
}

void CChangeAllDlg::OnBnClickedAnyAccentRadio()
{
	m_anyAccentModels = 1;
	updateParadigmList();
}
