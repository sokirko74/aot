// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VisualSynan.h"
#include "WaitDlg.h"
#include "MainFrm.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN_BUILDCLAUSESFROMFILE, OnBuildFromFile)
	ON_COMMAND(ID_FILE_REINITIALIZESYNTAX, OnReinitializeSyntax)
	
	ON_MESSAGE(ID_PROCESS_TXT_FILE, OnProcessTxtFile)
	ON_COMMAND(ID_SYN_HELP, OnSynHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bNewDoc = TRUE;	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{	
	cs.style |= SW_MAXIMIZE;
	//cs.dwExStyle |= SW_MAXIMIZE;
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnBuildFromFile() 
{	
	CString strFilter;
	strFilter.LoadString(IDS_PLM_FILTER);
	m_strCurFileName = GetFileWithThisFilter(strFilter);		
	if( m_strCurFileName.IsEmpty() )
		return;
	m_pWaitThread = new CWaitThread(m_hWnd);
	m_pWaitThread->CreateThread();
}

void CMainFrame::OnReinitializeSyntax() 
{	
	CWaitCursor r;
	CCOMSyntaxHolder& Holder = ((CVisualSynanApp*)AfxGetApp())->m_SyntaxHolder;
	Holder.LoadSyntaxModule(Holder.m_CurrentLanguage);
}




CString CMainFrame::GetFileWithThisFilter(CString strFilter)
{
	CFileDialog* p_dlgFile = new CFileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						strFilter);

	CString str;
	str.Empty();

	if( p_dlgFile->DoModal() != IDOK)
	{
		delete p_dlgFile;
		return str;
	}

	str = p_dlgFile->GetPathName();
	delete p_dlgFile;
	return str;
}

BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

LRESULT CMainFrame::OnProcessTxtFile(WPARAM, LPARAM)
{	
	m_bNewDoc = FALSE;
	::AfxGetApp()->OpenDocumentFile(m_strCurFileName);
	m_bNewDoc = TRUE;	
	m_pWaitThread->PostThreadMessage(TM_KILL,0,0);


	return 0;
}

void CMainFrame::OnSynHelp() 
{
	DWORD dummy = 0;
	::WinHelp(::AfxGetApp()->m_pMainWnd->m_hWnd, "syntax.hlp",HELP_FINDER, dummy);	
}

bool CMainFrame::LoadSyntaxByLanguage(MorphLanguageEnum t) 
{
	if (!((CVisualSynanApp*)AfxGetApp())->m_SyntaxHolder.LoadSyntax(t)) return false;

	int index_rus = m_wndToolBar.CommandToIndex(ID_RUSSIAN_SYNTAX);
	int index_ger = m_wndToolBar.CommandToIndex(ID_GERMAN_SYNTAX);
	int style = m_wndToolBar.GetButtonStyle(index_rus);
	if (t == morphRussian)
	{
		m_wndToolBar.SetButtonStyle(index_rus,TBBS_PRESSED);
		m_wndToolBar.SetButtonStyle(index_ger,style&(~TBBS_PRESSED));
	}
	else
	{
		m_wndToolBar.SetButtonStyle(index_ger,TBBS_PRESSED);
		m_wndToolBar.SetButtonStyle(index_rus,style&(~TBBS_PRESSED));
	}
	return true;
};