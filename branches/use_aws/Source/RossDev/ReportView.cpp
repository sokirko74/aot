// ReportView.cpp : implementation file
//

#include "stdafx.h"
#include "rossdev.h"
#include "ReportView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportView

IMPLEMENT_DYNCREATE(CReportView, CRichEditView)

CReportView::CReportView()
{
}

CReportView::~CReportView()
{
}


BEGIN_MESSAGE_MAP(CReportView, CRichEditView)
	//{{AFX_MSG_MAP(CReportView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportView drawing

extern CRossDevApp theApp;
void CReportView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
    m_Menu.LoadMenu(IDR_ROSSDETYPE);
	CWnd* Prev = (CWnd*)theApp.m_pMainWnd->SendMessage(WM_MDISETMENU, (WPARAM)0, (LPARAM)m_Menu.m_hMenu);
	GetParent()->SetMenu( &m_Menu );
	//SetScrollSizes(MM_TEXT, sizeTotal);
}

void CReportView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CReportView diagnostics

#ifdef _DEBUG
void CReportView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CReportView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportView message handlers



/*class CReportView : public CRichEditView 
{
 protected:
	DECLARE_DYNCREATE(CReportView)

	public:
	CMenu				 m_Menu;	
	CWnd* m_pParentWnd;
	CReportView() :CRichEditView() {};
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL)
	{
	   BOOL Result = 	CRichEditView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	   m_Menu.LoadMenu(IDR_ROSSDETYPE);
	   return Result;
	};
	virtual void OnActivateFrame( UINT nState, CFrameWnd* pFrameWnd );

	protected:
	DECLARE_MESSAGE_MAP()
	
};

  



void CReportView::OnActivateFrame( UINT nState, CFrameWnd* pFrameWnd )
{
		if (WA_ACTIVE   == nState) {
		   

		   //GetParent()->SetMenu( &m_Menu );
			theApp.m_pMainWnd->SendMessage(WM_MDISETMENU, (WPARAM)0, (LPARAM)m_Menu.m_hMenu);
			theApp.m_pMainWnd->DrawMenuBar();
			//m_pParentWnd->SetMenu( &m_Menu );
		   //theApp.m_pMainWnd->SetMenu( &m_Menu );
		};
};


*/
