// RossDev.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "resource.h"
#include "afxrich.h"
#include "RossDev.h"
#include <tk.h>
#include "MainFrm.h"
#include "ChildFrm.h"
#include "RossDoc.h"
#include "WordList.h"
#include "ArticleView.h"
#include "ArticleDoc.h"
#include "PocketDoc.h"
#include "ReportDoc.h"
#include "ReportView.h"
#include "PocketForm.h"
#include "HierarchyDoc.h"
#include "Hierarchy.h"
#include "SemanticStrView.h"
#include "SemanticStrDoc.h"

#include <initguid.h>
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRossDevApp

BEGIN_MESSAGE_MAP(CRossDevApp, CWinApp)
	//{{AFX_MSG_MAP(CRossDevApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_RUN_PROCEDURE_TO_BUILD_RUSSIAN_SEMANTIC_STRUCTURE, OnSemAn)
	ON_COMMAND(ID_OPEN_ALL_ROSSES, OnOpenAllRosses)
	ON_COMMAND(ID_SEMAN, OnFocusSeman)
	ON_COMMAND(ID_MINIMIZE_ALL_DICTS, OnMinimizeAllDicts)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

const char g_strFontNameRegPath[] = "Software\\Dialing\\Ross\\TextFontName";
const char g_strFontSizeRegPath[] = "Software\\Dialing\\Ross\\TextFontSize";


class CRossCommandLineInfo : public CCommandLineInfo
{
public:
	bool bSeman;
	bool bOnlyRoss;

	CRossCommandLineInfo() : CCommandLineInfo() {
		bSeman = false;
		bOnlyRoss = false;
	};

	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
	{
		CString S = pszParam;
		if (S == _T("seman")) bSeman = true;
		if (S == _T("onlyross")) bOnlyRoss = true;
		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
	};
};


/////////////////////////////////////////////////////////////////////////////
// CRossDevApp construction

CRossDevApp::CRossDevApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	CoInitialize(NULL);
	m_hEventMinusListReport = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_OpenRossDocReadOnly = false;
	m_bGraphanIsBusy = false;
}

CRossDevApp::~CRossDevApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance


	CoUninitialize();
	CloseHandle(m_hEventMinusListReport);

}
CRossDevApp theApp;


void CALLBACK TclTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	int ok = TRUE;
	while (ok) {
		ok = Tcl_DoOneEvent(TCL_ALL_EVENTS | TCL_DONT_WAIT);
	}
}

/////////////////////////////////////////////////////////////////////////////
void InitError()
{
	AfxMessageBox(_T("cannot initialize Tcl/Tk"));
}



/////////////////////////////////////////////////////////////////////////////
// The one and only CRossDevApp object



extern LRESULT CALLBACK NodeWindowFunc(HWND, UINT, WPARAM, LPARAM);
extern int SaveToRossdev(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
//extern int GetNewNode(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int Update(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int FindSituations(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int TranslateToEnglish(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);

extern int PasteClipboard(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int BuildSentence(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int SyntRusSentence(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);

extern int SaveSentence(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int ClearSavedSentences(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int AnswerBySavedSentences(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);

extern int CreateSynStr(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int GetOtherRelations(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int ShowArticle(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);
extern int ShowMessageMicrosoftWindows(ClientData clienData, Tcl_Interp* interp, int argc, char* argv[]);

void CRossDevApp::CreateTclCommand(char* name, Tcl_CmdProc proc) {
	Tcl_CreateCommand(m_TclInterp.TclInterp, name, proc, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

}

BOOL CRossDevApp::InitInstance()
{
	m_OnlyRoss = false;

	try {
		Tcl_SetServiceMode(TCL_SERVICE_ALL);
		m_TclInterp.InitializeTcl();
		m_TclInterp.InitializeTk();
		m_TclClassWnd = AfxRegisterWndClass(CS_NOCLOSE | CS_PARENTDC | CS_DBLCLKS);
		m_TclInterp.RunTcl("wm withdraw .");
		m_TclInterp.RunTcl(Format("set env(GRAPHLET_DIR) \"%s/Bin/Lib/graphlet\"", GetRmlVariable().c_str()));
	}
	catch (CExpc e) {
		AfxMessageBox(_U16(e.what()));
	}

	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	}

	AfxEnableControlContainer();

	AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


	// Change the registry key under which our settings are stored.
	// TODO: You should modify this std::string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(20);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_ROSSDETYPE,
		RUNTIME_CLASS(CRossDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CWordList));
	AddDocTemplate(pDocTemplate);



	pDocTemplate = new CMultiDocTemplate(
		IDR_ARTICLETYPE,
		RUNTIME_CLASS(CArticleDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CArticleView));
	AddDocTemplate(pDocTemplate);


	pDocTemplate = new CMultiDocTemplate(
		IDR_POCKETTYPE,
		RUNTIME_CLASS(CPocketDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPocketForm));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_HIERARCHYTYPE,
		RUNTIME_CLASS(CHierarchyDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CHierarchyView));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_REPORTTYPE,
		RUNTIME_CLASS(CReportDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CReportView));
	AddDocTemplate(pDocTemplate);


	pDocTemplate = new CMultiDocTemplate(
		IDR_SEMANTICSTRUCTURE,
		RUNTIME_CLASS(CSemanticStrDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSemanticStrView));
	AddDocTemplate(pDocTemplate);



	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	CRossCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	m_OnlyRoss = cmdInfo.bOnlyRoss;

	// инициализация досемантических словарей 
	m_SemBuilder.m_RusStr.m_pData->Init();
	//	m_pMainWnd->GetMenu( )->EnableMenuItem( ID_RUN_PROCEDURE_TO_BUILD_RUSSIAN_SEMANTIC_STRUCTURE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);


	// Dispatch commands specified on the command line
	if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew)
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;

	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();
	if (m_OnlyRoss)
		pMainFrame->GetMenu()->EnableMenuItem(ID_RUN_PROCEDURE_TO_BUILD_RUSSIAN_SEMANTIC_STRUCTURE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);



	//============================
	// определить класс окна для иерархии
	try
	{
		LOGFONT lf;                        // Used to create the CFont.
		memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
		std::string strFontSize = GetRegistryString(g_strFontSizeRegPath);
		lf.lfHeight = atoi(strFontSize.c_str());                  // Request a 20-pixel-high font
		lstrcpy(lf.lfFaceName, _U16(GetRegistryString(g_strFontNameRegPath)));    //    with face name "Arial".
		m_TextFont.CreateFontIndirect(&lf);    // Create the font.
	}
	catch (...)
	{
	};


	try {
		SetTimer(NULL, 42, 10, TclTimerProc);
		CreateTclCommand("SaveToRossdev", SaveToRossdev);
		//CreateTclCommand("GetNewNode", GetNewNode);
		CreateTclCommand("Update", Update);
		CreateTclCommand("FindSituations", FindSituations);
		CreateTclCommand("TranslateToEnglish", TranslateToEnglish);
		CreateTclCommand("PasteClipboard", PasteClipboard);
		CreateTclCommand("ShowMessageMicrosoftWindows", ShowMessageMicrosoftWindows);
		CreateTclCommand("BuildSentence", BuildSentence);
		CreateTclCommand("SyntRusSentence", SyntRusSentence);

		CreateTclCommand("SaveSentence", SaveSentence);
		CreateTclCommand("ClearSavedSentences", ClearSavedSentences);
		CreateTclCommand("AnswerBySavedSentences", AnswerBySavedSentences);

		CreateTclCommand("CreateSynStr", CreateSynStr);
		CreateTclCommand("GetOtherRelations", GetOtherRelations);
		CreateTclCommand("ShowArticle", ShowArticle);
		m_TclInterp.RunTcl(Format("source \"$env(GRAPHLET_DIR)/lib/graphscript/ross/init_gra.tcl\""));
		if (cmdInfo.bSeman)
			OnSemAn();
	}
	catch (std::exception& e) {
		AfxMessageBox(_U16(e.what()));
	}

	return TRUE;

}



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	void Do() {
		DoModal();
	};

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
void CRossDevApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.Do();
}

/////////////////////////////////////////////////////////////////////////////
// CRossDevApp message handlers



void CRossDevApp::OnFileOpen()
{
	// TODO: Add your command handler code here
	CWinApp::OnFileOpen();

}

DWORD CALLBACK EditStreamCallbackFromString(
	DWORD dwCookie, // application-defined value
	LPBYTE pbBuff,  // pointer to a buffer
	LONG cb,        // number of bytes to read or write
	LONG* pcb       // pointer to number of bytes transferred
)
{
	CString* S = (CString*)dwCookie;
	DWORD l = S->GetLength();
	*pcb = (cb < l) ? cb : l;
	if (*pcb > 0)
	{
		memcpy(pbBuff, *S, *pcb);
		*S = *(S->Right(l - *pcb));
		if (l == *pcb)
			*S = "";
	};
	return 0;
};


bool GlobalOpenReport(CString S, CString Name)
{
	CDocTemplate* T = GetReportTemplate();
	CReportDoc* pDocument = (CReportDoc*)T->CreateNewDocument();
	ASSERT_VALID(pDocument);
	T->InitialUpdateFrame(T->CreateNewFrame(pDocument, NULL), pDocument, TRUE);
	pDocument->m_bRTF = false;
	CRichEditCtrl& C = pDocument->GetView()->GetRichEditCtrl();
	pDocument->SetTitle(Name);
	S = Name + CString("\r\n\r\n") + S;
	C.SetWindowText(S);
	pDocument->InitFonts();
	return true;
};


BOOL CRossDevApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

void CRossDevApp::OnAppExit()
{
	// TODO: Add your command handler code here
	if (!SaveAllModified()) return;
	GetHierarchyTemplate()->CloseAllDocuments(TRUE);
	GetSemanticStructureTemplate()->CloseAllDocuments(TRUE);
	m_pMainWnd->SendMessage(WM_CLOSE);
}

BOOL CRossDevApp::OnIdle(LONG lCount)
{
	BOOL bMore = CWinApp::OnIdle(lCount);
	// TODO: Add your specialized code here and/or call the base class
	if (lCount == 0)
		if (WaitForSingleObject(m_hEventMinusListReport, 0) == WAIT_OBJECT_0)
		{
			ResetEvent(m_hEventMinusListReport);
			((CFrameWnd*)((CRossDevApp*)AfxGetApp())->m_pMainWnd)->SetMessageText(_T(""));
			GlobalOpenReport(m_strMinusListReport,
				"List of all lemmas that were not found in the dictionary");

		};

	return  bMore;
}


void CRossDevApp::OnSemAn()
{
	if (m_OnlyRoss)
	{
		AfxMessageBox(_T("Seman is disabled because of switch  -onlyross "));
		return;
	};

	CDocTemplate* T = GetSemanticStrTemplate();
	CSemanticStrDoc* pDocument = (CSemanticStrDoc*)T->CreateNewDocument();
	ASSERT_VALID(pDocument);


	try
	{

		BOOL bAutoDelete = pDocument->m_bAutoDelete;
		pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong

		CFrameWnd* pFrame = T->CreateNewFrame(pDocument, NULL);

		pDocument->m_bAutoDelete = bAutoDelete;
		if (pFrame == NULL)
		{
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
			delete pDocument;       // explicit delete on error
			return;
		};

		ASSERT_VALID(pFrame);

		T->InitialUpdateFrame(pFrame, pDocument, TRUE);
		pDocument->SetTitle(CString("Semantic Analysis "));

		CSemanticStrView* View = pDocument->GetView();


		// открывает все россы	
		View->OpenAllRosses();
	}
	catch (CExpc& c)
	{
		AfxMessageBox(_U16(c.what()));
		delete pDocument;
		return;

	}
	catch (...)
	{
		AfxMessageBox(_U16("Cannot create Semantic Structure Window"));
		delete pDocument;
		return;
	};

};

void CRossDevApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	if (lstrlen(lpszPathName) < 4) return;
	TCHAR s[4];
	s[3] = 0;
	lstrcpy(s, lpszPathName + lstrlen(lpszPathName) - 3);
	if (lstrcmp(s, _T("rcf"))) return;

	CWinApp::AddToRecentFileList(lpszPathName);
}




void CRossDevApp::OnOpenAllRosses()
{
	CSemanticStrView View;
	View.OpenAllRosses();
};

bool  FocusSemanWindow()
{
	try
	{
		CDocTemplate* pDocTemplate = GetSemanticStrTemplate();
		POSITION pos = pDocTemplate->GetFirstDocPosition();
		if (!pos) return false;
		CDocument* pDoc = pDocTemplate->GetNextDoc(pos);
		if (pDoc == 0) return false;
		pos = pDoc->GetFirstViewPosition();
		if (pos == 0) return false;
		CView* pView = pDoc->GetNextView(pos);
		CMDIChildWnd* Wnd = (CMDIChildWnd*)pView->GetParent();
		Wnd->MDIActivate();

	}
	catch (...)
	{
		AfxMessageBox(_T("Cannot find seman"));
		return false;
	}
	return true;

}

void CRossDevApp::OnFocusSeman()
{
	// TODO: Add your command handler code here
	FocusSemanWindow();
}

void CRossDevApp::OnMinimizeAllDicts()
{
	// TODO: Add your command handler code here
	CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
	POSITION pos = pRossDocTemplate->GetFirstDocPosition();
	while (pos)
	{
		CDocument* pDoc = pRossDocTemplate->GetNextDoc(pos);
		POSITION pos1 = pDoc->GetFirstViewPosition();
		if (pos1 == 0) return;
		CView* pView = pDoc->GetNextView(pos1);
		CMDIChildWnd* Wnd = (CMDIChildWnd*)pView->GetParent();
		Wnd->ShowWindow(SW_MINIMIZE);
	};
}



CRossDoc* CRossDevApp::FindRossDoc(DictTypeEnum rossType)
{
	CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
	assert(pRossDocTemplate);

	POSITION pos = pRossDocTemplate->GetFirstDocPosition();
	while (pos)
	{
		auto pDoc = dynamic_cast<CRossDoc*>(pRossDocTemplate->GetNextDoc(pos));
		if (pDoc != nullptr) {
			if (pDoc->GetRossType() == rossType) {
				return pDoc;
			}
		}
	}
	return nullptr;
}
