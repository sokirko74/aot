// DlgDelAnc.cpp : implementation file
//

#include "stdafx.h"
#include "DlgDelAnc.h"

#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDelAnc dialog


CDlgDelAnc::CDlgDelAnc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDelAnc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDelAnc)
	m_file = _T("");
	m_ancode = _T("");
	//}}AFX_DATA_INIT
	m_grm = 0;
}


void CDlgDelAnc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDelAnc)
	DDX_Text(pDX, IDC_FILE, m_file);
	DDV_MaxChars(pDX, m_file, 256);
	DDX_Text(pDX, IDC_ANCODE, m_ancode);
	DDV_MaxChars(pDX, m_ancode, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDelAnc, CDialog)
	//{{AFX_MSG_MAP(CDlgDelAnc)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDelAnc message handlers

BOOL CDlgDelAnc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO1+m_grm);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgDelAnc::OnBrowse() 
{
	CFileDialog dlgOpen(TRUE,NULL,NULL,
		OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,"All files(*.*)|*.*||");
	if( dlgOpen.DoModal() != IDOK )
		return;
	m_file = dlgOpen.GetPathName();
	GetDlgItem(IDC_FILE)->SetWindowText(m_file);
	GetDlgItem(IDC_FILE)->UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////

void CDlgDelAnc::OnOK() 
{
	m_grm = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2)==IDC_RADIO2;
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
