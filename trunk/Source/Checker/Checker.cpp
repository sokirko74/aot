// Checker.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Checker.h"
#include "CheckerDlg.h"

#include "GriIni.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckerApp

BEGIN_MESSAGE_MAP(CCheckerApp, CWinApp)
	//{{AFX_MSG_MAP(CCheckerApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckerApp construction

CCheckerApp::CCheckerApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCheckerApp object

CCheckerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCheckerApp initialization

BOOL CCheckerApp::InitInstance()
{

	CCheckerDlg dlg;
	m_pMainWnd = &dlg;
	{
			CString HelpName = AfxGetApp()->m_pszHelpFilePath;
			int pos = HelpName.ReverseFind('\\');
			ASSERT(pos != -1);
			HelpName = HelpName.Left(pos+1) + "Checker.opt";
			FILE*  fp = fopen (HelpName, "r");
			if (fp)
			{
				dlg.m_bLoadCOM = false;
				fclose (fp);
			}
	}

	CGriIni cIni("Loading COM...");
	cIni.Init();
	if( !dlg.InitCom() )
	{
		cIni.Exit();
		return FALSE;
	}
	cIni.Exit();

// Registry I/O
	SetRegistryKey("Gri");
	//dlg.m_file = GetProfileString("","File","");
//
	dlg.DoModal();

// Registry I/O
	//WriteProfileString("","File",dlg.m_file);

	dlg.ExitCom();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////

int CCheckerApp::ExitInstance() 
{
	CWinApp::ExitInstance();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
