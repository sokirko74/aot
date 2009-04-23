// VisualSynan.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WaitThread.h"
#include "VisualSynan.h"
#include "SynReport.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "VisualSynanDoc.h"
#include "VisualSynanView.h"
#include "Splash.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVisualSynanApp

BEGIN_MESSAGE_MAP(CVisualSynanApp, CWinApp)
	//{{AFX_MSG_MAP(CVisualSynanApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnSynFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// The one and only CVisualSynanApp object

CVisualSynanApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVisualSynanApp initialization


void CVisualSynanApp::OnSynFileNew()
{
	CDocTemplate* T = GetSynTemplate();
	CDocument* pDoc = T->CreateNewDocument();
	T->InitialUpdateFrame(T->CreateNewFrame(pDoc, NULL), pDoc, TRUE);
}


class CVisualSynanCommandLineInfo  : public CCommandLineInfo
{
public:
	MorphLanguageEnum m_Language;

	CVisualSynanCommandLineInfo () : CCommandLineInfo () {
		m_Language = morphRussian;
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




BOOL CVisualSynanApp::InitInstance()
{
	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	}

	CWaitThread::m_hEventKill
		= CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset, initially reset



	CoInitialize(NULL);


	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_VISUALTYPE,
		RUNTIME_CLASS(CVisualSynanDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVisualSynanView));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_REPORTTYPE,
		RUNTIME_CLASS(CReportDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CRichEditView));
	AddDocTemplate(pDocTemplate);


	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	CVisualSynanCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!pMainFrame->LoadSyntaxByLanguage(cmdInfo.m_Language))
		return FALSE;


	if( !cmdInfo.m_strFileName.IsEmpty() )
		pMainFrame->m_bNewDoc = FALSE;
	

	pMainFrame->m_bNewDoc = TRUE;
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);//m_nCmdShow);
	pMainFrame->UpdateWindow();
	OnSynFileNew();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CVisualSynanApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CVisualSynanApp message handlers


int CVisualSynanApp::ExitInstance() 
{
	CloseHandle(CWaitThread::m_hEventKill);
	m_SyntaxHolder.DeleteProcessors();
	CoUninitialize();
	return CWinApp::ExitInstance();
}

BOOL CVisualSynanApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}
