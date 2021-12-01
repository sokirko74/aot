// demo.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "demo.h"
#include "demoDlg.h"
#include "Splash.h"
#include "morph_dict/common/utilit.h"
#include "morph_dict/common/windows_unicode.h"



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
		if (GetLanguageByString(WstrToUtf8Str((const TCHAR*)S),l))
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
		AfxMessageBox(_T("Fatal Error: windows initialization failed"));
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
