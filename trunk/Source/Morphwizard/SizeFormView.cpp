// SizeFormView.cpp : implementation file
//

#include "stdafx.h"
//#include <afx.h>
//#include <afxwin.h>
//#include <..\mfc\src\afximpl.h>
#include <../atlmfc/src/mfc/afximpl.h>
#include "SizeFormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSizeFormView//

IMPLEMENT_DYNAMIC(CSizeFormView, CFormView)

CSizeFormView::CSizeFormView( LPCTSTR lpszTemplateName ) :
        CFormView(lpszTemplateName)
{
    m_hGadgetResize = NULL;
}

CSizeFormView::CSizeFormView( UINT nIDTemplate ) :
        CFormView(nIDTemplate)
{
    m_hGadgetResize = NULL;
}

CSizeFormView::~CSizeFormView()
{
    DestroyGadgetResizeHandle(m_hGadgetResize);
}

BEGIN_MESSAGE_MAP(CSizeFormView, CFormView)
    //{{AFX_MSG_MAP(CSizeFormView)
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSizeFormView diagnostics

#ifdef _DEBUG
void CSizeFormView::AssertValid() const
{
    CFormView::AssertValid();
}

void CSizeFormView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSizeFormView message handlers

BOOL CSizeFormView::Create(LPCTSTR /*lpszClassName*/, LPCTSTR /*lpszWindowName*/,
    DWORD dwRequestedStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
    CCreateContext* pContext)
{
    ASSERT(pParentWnd != NULL);
    ASSERT(m_lpszTemplateName != NULL);

    m_pCreateContext = pContext;    // save state for later OnCreate

    // initialize common controls
    //VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTLS_REG));
    AfxDeferRegisterClass(AFX_WNDCOMMCTLSNEW_REG);

    // call PreCreateWindow to get prefered extended style
    CREATESTRUCT cs; memset(&cs, 0, sizeof(CREATESTRUCT));
    if (dwRequestedStyle == 0)
        dwRequestedStyle = AFX_WS_DEFAULT_VIEW;
    cs.style = dwRequestedStyle;
    if (!PreCreateWindow(cs))
        return FALSE;

    // create a modeless dialog
    if (!CreateDlg(m_lpszTemplateName, pParentWnd))
        return FALSE;

    m_pCreateContext = NULL;

    // we use the style from the template - but make sure that
    //  the WS_BORDER bit is correct
    // the WS_BORDER bit will be whatever is in dwRequestedStyle
    ModifyStyle(WS_BORDER|WS_CAPTION, cs.style & (WS_BORDER|WS_CAPTION));
    ModifyStyleEx(WS_EX_CLIENTEDGE, cs.dwExStyle & WS_EX_CLIENTEDGE);

    SetDlgCtrlID(nID);

    m_hGadgetResize = CreateGadgetResizeHandle(this);

    CRect rectTemplate;
    GetWindowRect(rectTemplate);
    SetScaleToFitSize(rectTemplate.Size());

    // initialize controls etc
    if (!ExecuteDlgInit(m_lpszTemplateName))
        return FALSE;

    // force the size requested
    SetWindowPos(NULL, rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOZORDER|SWP_NOACTIVATE);

    // make visible if requested`
    if (dwRequestedStyle & WS_VISIBLE)
        ShowWindow(SW_NORMAL);

    return TRUE;
}

void CSizeFormView::OnSize(UINT nType, int cx, int cy)
{
    if (m_hGadgetResize)
        ResizeGadgetsOnWindow(m_hGadgetResize, cx, cy);
    else
        CFormView::OnSize(nType, cx, cy);
}

