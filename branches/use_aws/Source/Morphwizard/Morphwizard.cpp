// Morphwizard.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Morphwizard.h"
#include "MainFrm.h"
#include <direct.h>

#include "ChildFrm.h"
#include "MorphwizardDoc.h"
#include "MorphwizardView.h"

#include "SLFDocument.h"
#include "SLFView.h"
#include "ParadigmDifferences.h"

#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#endif



// CMorphwizardApp

BEGIN_MESSAGE_MAP(CMorphwizardApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
END_MESSAGE_MAP()



class CMWZCommandLineInfo  : public CCommandLineInfo
{
public:

	CMWZCommandLineInfo () : CCommandLineInfo () {
	};
	
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
	{
		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
	};
};


// CMorphwizardApp construction

CMorphwizardApp::CMorphwizardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMorphwizardApp object

CMorphwizardApp theApp;

// CMorphwizardApp initialization

BOOL CMorphwizardApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();


	AfxInitRichEdit2 ();

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_MorphwizardTYPE,
		RUNTIME_CLASS(CMorphwizardDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMorphwizardView));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_SLF_TYPE,
		RUNTIME_CLASS(CSLFDocument),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSLFView));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DIFF_TYPE,
		RUNTIME_CLASS(CSLFDocument),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CParadigmDifferences));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	int Id = IDR_MAIN_FRAME;
	if (!pMainFrame->LoadFrame(Id))
		return FALSE;
	m_pMainWnd = pMainFrame;


	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Parse command line for standard shell commands, DDE, file open
	CMWZCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew)
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;

	// The main window has been initialized, so show and update it
//	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CMorphwizardApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMorphwizardApp message handlers


void CMorphwizardApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	
	std::string q = lpszPathName;
	if (q.find("mwz") == std::string::npos)
		return;

	CWinApp::AddToRecentFileList(lpszPathName);
}


void CMorphwizardApp::OnHelpHelp()
{
		AfxGetApp()->WinHelp(IDH_LOADING);
}

//----------------------------------------------------------------------------
void SetInputLanguage( MorphLanguageEnum lang )
{
	//  Keyboard switching is disabled because it makes me nervous.  Alexey Sokirko
	

	UINT kbdFlags = KLF_ACTIVATE;//|KLF_REORDER|KLF_SUBSTITUTE_OK|KLF_NOTELLSHELL;
	HKL hkl=0;
	switch( lang ) 
	{
	case morphRussian:
		//hkl=::LoadKeyboardLayout(_T("00000419"),kbdFlags);				//Russian
		::SetThreadLocale(MAKELCID(MAKELANGID(LANG_RUSSIAN,SUBLANG_DEFAULT),SORT_DEFAULT));
		break;
	case morphGerman:	
		/*if( (hkl=::LoadKeyboardLayout(_T("00000407"),kbdFlags))==0 )	//German (Standard)
			hkl=::LoadKeyboardLayout(_T("00000007"),kbdFlags);			//German (neutral)*/
		::SetThreadLocale(MAKELCID(MAKELANGID(LANG_GERMAN,SUBLANG_DEFAULT),SORT_DEFAULT));
		break;
	case morphEnglish:	
	default: 
		/*if( (hkl=::LoadKeyboardLayout(_T("00000409"),kbdFlags))==0 )	//English (United States)
			hkl=::LoadKeyboardLayout(_T("00000009"),kbdFlags);			//English (neutral)*/
		::SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT),SORT_DEFAULT));
	}
}
