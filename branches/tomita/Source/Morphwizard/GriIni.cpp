/////////////////////////////////////////////////////////////////////////////
// Class CGriIni - realizes Loading Message
// Version 2.0 Gri
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GriIni.h"

#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGriIni

CGriIni::CGriIni(LPCTSTR message)
{
	if( message )
		m_text = message;
	else
		m_text = "Loading, please wait...";
}

CGriIni::~CGriIni()
{
}

BEGIN_MESSAGE_MAP(CGriIni, CWnd)
	//{{AFX_MSG_MAP(CGriIni)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CGriIni::Init(CWnd *pWnd)
{
	int sx,sy,dx,dy;
	dx = 206;
	sx = (GetSystemMetrics(SM_CXSCREEN) - dx)/2;
	dy = 106;
	sy = (GetSystemMetrics(SM_CYSCREEN) - dy)/2;
//
	if( !CWnd::CreateEx(WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(0),"",WS_POPUP,
		sx,sy,dx,dy,
		pWnd->GetSafeHwnd(),NULL,NULL) )
	{
		return( FALSE );
	}
//
	CenterWindow();
	ShowWindow(SW_SHOWNORMAL);
	UpdateWindow();

	SetCursor(LoadCursor(NULL,IDC_WAIT));
	return( TRUE );
}

//////////////////////////////////////////////////////////////////////////////

void CGriIni::Exit(void)
{
	SendMessage(WM_CLOSE);
}

//////////////////////////////////////////////////////////////////////////////
// CGriIni message handlers


void CGriIni::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

// Shadow
	CRgn rgn1;
	rgn1.CreateRectRgn(rect.left+6,rect.bottom-6,
			rect.right,rect.bottom);
	CRgn rgn2;
	rgn2.CreateRectRgn(rect.right-6,rect.top+6,
			rect.right,rect.bottom);
	rgn2.CombineRgn(&rgn1,&rgn2,RGN_OR);

	WORD pBits[8] = { 0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA };
	CBitmap bitmap;
	bitmap.CreateBitmap(8,8,1,1,pBits);
	CBrush  shadow(&bitmap);

	int oldRop = dc.SetROP2(R2_MASKPEN);
	dc.FillRgn(&rgn2,&shadow);
	dc.SetROP2(oldRop);

	rgn1.DeleteObject();
	rgn2.DeleteObject();

// Black Outline
	rect.right  -= 6;
	rect.bottom -= 6;

	CBrush black(RGB(0,0,0));
	dc.FrameRect(&rect,&black);

// Fill & Gray Outline
	rect.DeflateRect(1,1);
	CBrush brush(RGB(255,255,238));
	brush.UnrealizeObject();
	dc.SetBrushOrg(4,4);
	CBrush *oldBsh = dc.SelectObject(&brush);
	CPen pen(PS_SOLID,1,RGB(192,192,192));
	CPen *oldPen = dc.SelectObject(&pen);
	dc.Rectangle(&rect);
	dc.SelectObject(oldPen);
	dc.SelectObject(oldBsh);

// Text
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,0));
	dc.DrawText(m_text,&rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////
