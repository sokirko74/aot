// WaitThread.cpp: implementation of the CWaitThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VisualSynan.h"
#include "WaitThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


HANDLE CWaitThread::m_hEventKill;

BEGIN_MESSAGE_MAP(CWaitThread, CWinThread)
	//{{AFX_MSG_MAP(CBounceThread)
	ON_THREAD_MESSAGE(TM_KILL, OnKill)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWaitThread, CWinThread)

CWaitThread::CWaitThread(HWND hwndParent) : m_hwndParent(hwndParent)
{	
	m_pWaitDlg = NULL;
}

CWaitThread::~CWaitThread()
{
	if( m_pWaitDlg )
		delete m_pWaitDlg;
}

BOOL CWaitThread::InitInstance()
{	
	CWnd* pParent = CWnd::FromHandle(m_hwndParent);
	m_bAutoDelete = TRUE;
	m_pWaitDlg = new CWaitAsyncDialog(pParent);
	m_pWaitDlg->Create();	
	m_pMainWnd = m_pWaitDlg;
	pParent->PostMessage(ID_PROCESS_TXT_FILE);
	return TRUE;
}


int  CWaitThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}


void CWaitThread::KillThread()
{
}

void	CWaitThread::OnKill(WPARAM, LPARAM)
{	
	PostQuitMessage(0);
}



BOOL CWaitThread::OnIdle(long lCount)
{
	return CWinThread::OnIdle(lCount);
}
