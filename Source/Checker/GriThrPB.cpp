/////////////////////////////////////////////////////////////////////////////
// Class 'Thread Progress Bar'
// Version 1.10 Gri
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "GriThrPB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThrPB dialog

#ifdef _DEBUG
	IMPLEMENT_DYNAMIC(CThrPB, CDialog)
#endif

CThrPB::CThrPB(CWnd* pParent,
		AFX_THREADPROC pfnProc, // UINT (*pfn)(LPVOID)
		LPVOID pData)
	: CDialog(CThrPB::IDD, pParent)
{
	ASSERT( pfnProc );
	m_pfnProc = pfnProc;
	m_pData = pData;
	m_pText = NULL;
	m_fBreak = FALSE;
	m_iValue = 0;
	m_cHandle = NULL;
}

BEGIN_MESSAGE_MAP(CThrPB, CDialog)
	//{{AFX_MSG_MAP(CThrPB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The CThrPB pointer

CThrPB* theTpb;

/////////////////////////////////////////////////////////////////////////////
// CThrPB Code

void CThrPB::SetHead(LPCTSTR pText)
{
	ASSERT( pText );
	m_pText = pText;

	ASSERT( IsWindow(m_hWnd) );
	if( IsWindow(m_hWnd) )
		SetWindowText(m_pText);
}

/////////////////////////////////////////////////////////////////////////////

static LPCTSTR fmt = "%3i %% done";
static char   buf[12];

void CThrPB::Percent(int iValue)
{
	ASSERT( iValue>=0 && iValue<=100 );
	iValue = (max(0,min(100,iValue)));
	if( iValue == m_iValue )
		return;
	m_iValue = iValue;

	ASSERT( IsWindow(m_hWnd) );
	if( !IsWindow(m_hWnd) )
		return;

	wsprintf(buf,fmt,iValue);
	SetDlgItemText(IDC_TPB_ST,buf);
	GetDlgItem(IDC_TPB_PB)->SendMessage(PBM_SETPOS,iValue,0L);
}

//////////////////////////////////////////////////////////////////////////////

BOOL CThrPB::IsBreak(void)
{
	return( m_fBreak );
}

//////////////////////////////////////////////////////////////////////////////

void CThrPB::EndDone(void)
{
	ASSERT( IsWindow(m_hWnd) );
	SendMessage(WM_COMMAND,IDOK,0L);
//	AfxEndThread(1);
}

//////////////////////////////////////////////////////////////////////////////

void CThrPB::EndExit(void)
{
	ASSERT( IsWindow(m_hWnd) );
	SendMessage(WM_COMMAND,IDABORT,0L);
//	AfxEndThread(0);
}

/////////////////////////////////////////////////////////////////////////////
// CThrPB message handlers

BOOL CThrPB::OnInitDialog()
{
	CDialog::OnInitDialog();
//
	m_fBreak = FALSE;
	m_iValue = 0;
//
	GetDlgItem(IDC_TPB_PB)->SendMessage(PBM_SETSTEP,5,0L);
	if( m_pText )
		SetWindowText(m_pText);
	UpdateWindow();
//
	m_cHandle = AfxBeginThread(m_pfnProc,m_pData);
//
	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////

BOOL CThrPB::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD(wParam) )
	{
		case IDABORT:
			EndDialog(0);
		return( TRUE );
		case IDOK:
			EndDialog(1);
		return( TRUE );
		case IDCANCEL:
			m_fBreak = TRUE;
		return( TRUE );
	}

	return CDialog::OnCommand(wParam,lParam);
}

/////////////////////////////////////////////////////////////////////////////
