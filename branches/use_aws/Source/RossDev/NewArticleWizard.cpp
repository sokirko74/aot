// NewArticleWizard.cpp : implementation file
//

#include "stdafx.h"
#include "..\rossdev\resource.h"
#include "NewArticleWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewArticleWizard

IMPLEMENT_DYNAMIC(CNewArticleWizard, CPropertySheet)



CNewArticleWizard::CNewArticleWizard(LPCTSTR pszCaption, CRossDoc* pRossDoc)
	:CPropertySheet(pszCaption, 0, 0)
{

	m_pRossDoc = pRossDoc;
}

CNewArticleWizard::~CNewArticleWizard()
{
	for (size_t i=3; i< GetPageCount(); i++)
	 delete GetPage(i);
}


BEGIN_MESSAGE_MAP(CNewArticleWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CNewArticleWizard)
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewArticleWizard message handlers

void CNewArticleWizard::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CPropertySheet::OnActivate(nState, pWndOther, bMinimized);
	
	// TODO: Add your message handler code here
	
}
BOOL CNewArticleWizard::OnSetActive( )
{
	SetWizardButtons( PSWIZB_BACK|PSWIZB_NEXT|PSWIZB_FINISH);
    SetFinishText("Finish");
	return TRUE;
};

BOOL CNewArticleWizard::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.cx = 100;
	cs.cy = 100;
	return CPropertySheet::PreCreateWindow(cs);
}
