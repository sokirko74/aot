#include "StdAfx.h"
#include "VisualSynan.h"
#include "MainFrm.h"
#include "SynReport.h"
#include "DlgHom.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_RUN_SYNTAX, OnRunSyntax)
	ON_COMMAND(ID_GERMAN_SYNTAX, OnGermanSyntax)
	ON_COMMAND(ID_RUSSIAN_SYNTAX, OnRussianSyntax)
	ON_UPDATE_COMMAND_UI(ID_RUN_SYNTAX, OnUpdateRunSyntax)
	ON_UPDATE_COMMAND_UI(ID_RUSSIAN_SYNTAX, OnUpdateRunSyntax)
	ON_UPDATE_COMMAND_UI(ID_GERMAN_SYNTAX, OnUpdateRunSyntax)
	ON_COMMAND(ID_BUILD_RELS, OnBuildRels)
	ON_COMMAND(IDM_KILLHOM, OnKillHom)
	ON_COMMAND(ID_SHOW_MESSAGES, OnShowMessages)

	

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CChildFrame::CChildFrame()
{
	m_bFirst = TRUE;
	m_bNewDoc = FALSE;
	m_pSynView = NULL;
	m_pEditView = NULL;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//cs.style |= (WS_VISIBLE & WS_MINIMIZE
	
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, uint32_t dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{	
	/*if( !CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext) )
		return FALSE;*/
	
	return CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
	//ModifyStyle(WS_MAXIMIZE, WS_MAXIMIZE);
}

void CChildFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	ShowWindow(SW_SHOWMAXIMIZED);
}	//CMDIChildWnd::OnShowWindow(bShow, nStatus);

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	
	if( pContext->m_pNewViewClass->IsDerivedFrom(RUNTIME_CLASS(CRichEditView)) )
	{
		return CMDIChildWnd::OnCreateClient(lpcs, pContext);
	}

	if( !((CMainFrame*)::AfxGetMainWnd())->m_bNewDoc )	
	{
		m_pSynView = (CVisualSynanView *)pContext->m_pLastView;
		return CMDIChildWnd::OnCreateClient(lpcs, pContext);
	}
	else
	{
		m_wndSplitter.CreateStatic(this, 2, 1);
		CRect rect;
		GetWindowRect(&rect);
		


		m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CEditView), CSize(rect.right, rect.bottom/2), pContext);
		m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CVisualSynanView), CSize(rect.right, rect.bottom/2), pContext);

		m_bNewDoc = TRUE;
		m_pEditView =	(CEditView*)m_wndSplitter.GetPane(0,0); 
		
		try 
		{
			/*LOGFONT lf;                        // Used to create the CFont.
			memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
			std::string strFontSize = GetRegistryString( g_strFontSizeRegPath);
			lf.lfHeight = atoi(strFontSize.c_str());                  // Request a 20-pixel-high font
			strcpy(lf.lfFaceName, GetRegistryString( g_strFontNameRegPath ).c_str());    //    with face name "Arial".
			m_Font.CreateFontIndirect(&lf);    // Create the font.
			m_pEditView->SetFont (&m_Font);*/
			m_pSynView = (CVisualSynanView*)m_wndSplitter.GetPane(1,0);
       
		}
		catch  (...)
		{
		};
 
        

		

		return TRUE;
	}
}

void CChildFrame::OnBuildRels()
{
	CString report;
	((CVisualSynanView *)GetActiveView())->OnBuildRels(report);	
	GlobalOpenReport(report, "Relations");
}

void CChildFrame::OnRunSyntax() 
{
	ASSERT(m_pSynView && m_pEditView);
	m_pSynView->Reset();

	std::wstring str = m_pEditView->LockBuffer();
	m_pEditView->UnlockBuffer();

	int nBeg,nEnd;
	m_pEditView->GetEditCtrl().GetSel(nBeg,nEnd);
	if( nBeg<nEnd )
		str = str.substr(nBeg,nEnd-nBeg);

	CString strText = str.c_str();
	
	CVisualSynanDoc* pDoc = m_pSynView->GetDocument();	
	if( !pDoc->ProcessString(strText) )
		return;

	m_pSynView->Invalidate();	
}

void CChildFrame::OnRussianSyntax()
{
	CMainFrame*p = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	p->LoadSyntaxByLanguage(morphRussian);
	

};
void CChildFrame::OnGermanSyntax()
{
	CMainFrame*p = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	p->LoadSyntaxByLanguage(morphGerman);

};
void CChildFrame::OnUpdateRunSyntax(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_bNewDoc );
}

/////////////////////////////////////////////////////////////////////////////

void CChildFrame::OnKillHom() 
{
	CVisualSynanApp *pApp = (CVisualSynanApp *)AfxGetApp();

	CDlgHom dlg(this);
	dlg.m_type = pApp->GetHolder().m_Synan.GetOpt()->m_KillHomonymsMode;
	
	if( dlg.DoModal() != IDOK )
		return;

	pApp->GetHolder().m_Synan.SetKillHomonymsMode((KillHomonymsEnum)dlg.m_type);
}


BOOL bShowMessages = TRUE;

void CChildFrame::OnShowMessages() 
{
	// TODO: Add your command handler code here
	CVisualSynanApp *pApp = (CVisualSynanApp *)AfxGetApp();

	bShowMessages = !bShowMessages;
	pApp->GetHolder().m_Synan.put_SilentMode(!bShowMessages);
	CMenu* pMenu = GetParent()->GetParent()->GetMenu();
	if (bShowMessages)
	  pMenu->CheckMenuItem (ID_SHOW_MESSAGES,	MF_BYCOMMAND  | MF_CHECKED );
	else
	  pMenu->CheckMenuItem (ID_SHOW_MESSAGES,	MF_BYCOMMAND  | MF_UNCHECKED );
	
}



