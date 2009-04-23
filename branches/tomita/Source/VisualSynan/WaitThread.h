// WaitThread.h: interface for the CWaitThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAITTHREAD_H__0B5590B3_2E16_11D3_8C59_00105A68ADF3__INCLUDED_)
#define AFX_WAITTHREAD_H__0B5590B3_2E16_11D3_8C59_00105A68ADF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "thread.h"
#include "waitdlg.h"

#define TM_KILL 13890
#define ID_PROCESS_TXT_FILE 13891

class CWaitThread  : public CWinThread
{
	DECLARE_DYNCREATE(CWaitThread)
public:
	CWaitThread() {};
	CWaitThread(HWND Parent);
	virtual ~CWaitThread();
	//virtual BOOL Destroy();
	//void Create(long dwFlags);
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	void KillThread();
	BOOL OnIdle(long lCount);
	static HANDLE m_hEventKill;

protected:
	//virtual BOOL Run();
	CWaitAsyncDialog* m_pWaitDlg;
	HWND m_hwndParent;		

protected: 
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnKill(WPARAM, LPARAM);
	//}}AFX_MSG


	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WAITTHREAD_H__0B5590B3_2E16_11D3_8C59_00105A68ADF3__INCLUDED_)
