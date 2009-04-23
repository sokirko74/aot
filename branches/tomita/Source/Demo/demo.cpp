// demo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "demo.h"
#include "demoDlg.h"
#include "Splash.h"
#include "../common/utilit.h"

#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemoApp


BEGIN_MESSAGE_MAP(CDemoApp, CWinApp)
	//{{AFX_MSG_MAP(CDemoApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemoApp construction

class CDemoLineInfo  : public CCommandLineInfo
{
public:
	MorphLanguageEnum m_Language;

	CDemoLineInfo () : CCommandLineInfo () {
		m_Language = morphUnknown;
	};
	
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
	{
		CString S = pszParam;
		MorphLanguageEnum l;
		if (GetLanguageByString((const char*)S,l))
			m_Language = l;
		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
	};
};



CDemoApp::CDemoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDemoApp object

CDemoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDemoApp initialization

BOOL CDemoApp::InitInstance()
{
	if (FAILED(::CoInitialize(NULL)))
	{
		AfxMessageBox("Fatal Error: COM initialization failed");
		return FALSE;
	}

	CDemoLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	m_Language = cmdInfo.m_Language;

	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CDemoDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CDemoApp::ExitInstance() 
{
	::CoUninitialize();
	return CWinApp::ExitInstance();
}

BOOL CDemoApp::PreTranslateMessage(MSG* pMsg) 
{
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;
	
	return CWinApp::PreTranslateMessage(pMsg);
}
