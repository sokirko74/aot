// ProgressForm.cpp : implementation file
//

#include "stdafx.h"
#include "rossdev.h"
#include "resource.h"
#include "ProgressForm.h"
#include "SemanticStrView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressForm dialog


CProgressForm::CProgressForm(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressForm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressForm)
	m_Statis = _T("");
	//}}AFX_DATA_INIT
}


void CProgressForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressForm)
	DDX_Control(pDX, IDC_PROGRESSBAR, m_ProgressCtrl);
	DDX_Text(pDX, IDC_Statis, m_Statis);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressForm, CDialog)
	//{{AFX_MSG_MAP(CProgressForm)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressForm message handlers

extern bool ThreadFinish;
void CProgressForm::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
    if (ThreadFinish) EndDialog(0);

	
	if (m_CurrentPos == -1)
		{
			if (GetSemBuilder().GetScrollMax() > 0)
			{
				m_Max = GetSemBuilder().GetScrollMax();
				m_ProgressCtrl.SetRange(0, m_Max);
				m_CurrentPos = 0;
			};
		}
		else
		{
			if (GetSemBuilder().GetScrollCurrent() > m_CurrentPos)
			{
				m_CurrentPos = GetSemBuilder().GetScrollCurrent();
				m_ProgressCtrl.SetPos(m_CurrentPos);
				m_Statis.Format ("%i/%i", m_CurrentPos, m_Max);
				UpdateData(FALSE);
			};

		};
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CProgressForm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_CurrentPos = -1;
	m_Max = 0;
	SetTimer(2, 1000, 0);
	

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProgressForm::OnCancel() 
{
	// TODO: Add extra cleanup here
	GetSemBuilder().m_RusStr.m_bShouldBeStopped = true;
	//CDialog::OnCancel();
}
