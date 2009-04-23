// WaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitAsyncDialog dialog


CWaitAsyncDialog::CWaitAsyncDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitAsyncDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}



CWaitAsyncDialog::~CWaitAsyncDialog() 
{
	if ((m_pParentWnd)&&
		(m_pParentWnd->m_hWnd != NULL))
		m_pParentWnd->EnableWindow(TRUE);
}


BOOL CWaitAsyncDialog::OnInitDialog( )
{
	BOOL bResult = CDialog::OnInitDialog();
	ASSERT(m_pParentWnd);
	CRect parentRect;
	m_pParentWnd->GetWindowRect(parentRect);
	CRect thisRect;
	GetWindowRect(thisRect);
	int deltaX = (parentRect.Width() - thisRect.Width())/2;
	int deltaY = (parentRect.Height() - thisRect.Height())/2;
	SetWindowPos(NULL, parentRect.TopLeft().x + deltaX, parentRect.TopLeft().y + deltaY, 0, 0, SWP_HIDEWINDOW|
		SWP_NOSIZE|SWP_NOZORDER);
	m_WaitAnimation.Open(IDR_AVIWAIT);
	return (bResult);
}

void CWaitAsyncDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitAsyncDialog)
	DDX_Control(pDX, IDC_ANIMATE_WAIT, m_WaitAnimation);
	//}}AFX_DATA_MAP
}

BOOL CWaitAsyncDialog::Create()
{
	return CDialog::Create(m_lpszTemplateName, m_pParentWnd);
}

BEGIN_MESSAGE_MAP(CWaitAsyncDialog, CDialog)
	//{{AFX_MSG_MAP(CWaitAsyncDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


