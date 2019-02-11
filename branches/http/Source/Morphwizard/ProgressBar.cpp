// ProgressBar.cpp : implementation file
//
// Drop-in status bar progress control
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in this file is used in any commercial application 
// then an email to me would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage in any way,
// shape or form.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file.
//
// Version 1.1  Chris Maunder (chrismaunder@codeguru.com)
//
// Version 1.2  Michael Martin 07 Aug 1998	mmartin@netspace.net.au
//              Added code to enable progress bar to appear in any pane
//              of the status bar.
//
//              Chris Maunder 17 Aug 1998 
//              Fixed Pane text restoration, and general cleanup.
//
/////////////////////////////////////////////////////////////////////////////
	
#include "stdafx.h"
#include "ProgressBar.h"

#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CProgressBar, CProgressCtrl)

BEGIN_MESSAGE_MAP(CProgressBar, CProgressCtrl)
	//{{AFX_MSG_MAP(CProgressBar)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CProgressBar::CProgressBar()
{
	m_Rect.SetRect(0,0,0,0);
}

CProgressBar::CProgressBar(LPCTSTR strMessage, int nSize /*=100*/, 
						   int MaxValue /*=100*/, BOOL bSmooth /*=FALSE*/, 
                           int nPane /*=0*/)
{
	Create(strMessage, nSize, MaxValue, bSmooth, nPane);
}

CProgressBar::~CProgressBar()
{
	Clear();
}

CStatusBar* CProgressBar::GetStatusBar()
{
	CWnd *pMainWnd = AfxGetMainWnd();
	if (!pMainWnd)
		return NULL;

    // If main window is a frame window, use normal methods...
    if (pMainWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
    {
        CWnd* pMessageBar = ((CFrameWnd*)pMainWnd)->GetMessageBar();
        return DYNAMIC_DOWNCAST(CStatusBar, pMessageBar);
    }
    // otherwise traverse children to try and find the status bar...
    else
	    return DYNAMIC_DOWNCAST(CStatusBar, 
                                pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR));
}

// Create the CProgressCtrl as a child of the status bar positioned
// over the first pane, extending "nSize" percentage across pane.
// Sets the range to be 0 to MaxValue, with a step of 1.
BOOL CProgressBar::Create(LPCTSTR strMessage, int nSize /*=100*/, 
						  int MaxValue /*=100*/, BOOL bSmooth /*=FALSE*/, 
                          int nPane /*=0*/)
{
	BOOL bSuccess = FALSE;
	
	CStatusBar *pStatusBar = GetStatusBar();
	if (!pStatusBar)
		return FALSE;

	DWORD dwStyle = WS_CHILD|WS_VISIBLE;
#ifdef PBS_SMOOTH	 
	if (bSmooth)
		dwStyle |= PBS_SMOOTH;
#endif

	// Get CRect coordinates for requested status bar pane
	CRect PaneRect;
	pStatusBar->GetItemRect(nPane, &PaneRect);

	// Create the progress bar
	bSuccess = CProgressCtrl::Create(dwStyle, PaneRect, pStatusBar, 1);
	ASSERT(bSuccess);
	if (!bSuccess)
		return FALSE;

	// Set range and step
	SetRange(0, MaxValue);
	SetStep(1);

	m_strMessage  = strMessage;
	m_nSize 	  = nSize;
	m_nPane 	  = nPane;
    m_strPrevText = pStatusBar->GetPaneText(m_nPane); 

	// Resize the control to its desired width
	Resize();

	return TRUE;
}

void CProgressBar::Clear()
{
	if (!IsWindow(GetSafeHwnd()))
		return;

	// Hide the window. This is necessary so that a cleared
	// window is not redrawn if "Resize" is called
	ModifyStyle(WS_VISIBLE, 0);

	CString str;
	if (m_nPane == 0)
		str.LoadString(AFX_IDS_IDLEMESSAGE);   // Get the IDLE_MESSAGE
      else
		str = m_strPrevText;                   // Restore previous text

	// Place the IDLE_MESSAGE in the status bar 
	CStatusBar *pStatusBar = GetStatusBar();
	if (pStatusBar)
	{
		pStatusBar->SetPaneText(m_nPane, str);
		pStatusBar->UpdateWindow();
	}
}

BOOL CProgressBar::SetText(LPCTSTR strMessage)
{ 
	m_strMessage = strMessage; 
	return Resize(); 
}

BOOL CProgressBar::SetSize(int nSize)
{
	m_nSize = nSize; 
	return Resize();
}

COLORREF CProgressBar::SetBarColour(COLORREF clrBar)
{
#ifdef PBM_SETBKCOLOR
	if (!IsWindow(GetSafeHwnd()))
          return CLR_DEFAULT;

	return SendMessage(PBM_SETBARCOLOR, 0, (LPARAM) clrBar);
#else
	UNUSED(clrBar);
	return CLR_DEFAULT;
#endif
}

COLORREF CProgressBar::SetBkColour(COLORREF clrBk)
{
#ifdef PBM_SETBKCOLOR
	if (!IsWindow(GetSafeHwnd()))
		return CLR_DEFAULT;

	return SendMessage(PBM_SETBKCOLOR, 0, (LPARAM) clrBk);
#else
	UNUSED(clrBk);
	return CLR_DEFAULT;
#endif
}

BOOL CProgressBar::SetRange(int nLower, int nUpper, int nStep /* = 1 */)	
{	 
	if (!IsWindow(GetSafeHwnd()))
		return FALSE;

	// To take advantage of the Extended Range Values we use the PBM_SETRANGE32
	// message intead of calling CProgressCtrl::SetRange directly. If this is
	// being compiled under something less than VC 5.0, the necessary defines
	// may not be available.

#ifdef PBM_SETRANGE32
	ASSERT(-0x7FFFFFFF <= nLower && nLower <= 0x7FFFFFFF);
	ASSERT(-0x7FFFFFFF <= nUpper && nUpper <= 0x7FFFFFFF);
	SendMessage(PBM_SETRANGE32, (WPARAM) nLower, (LPARAM) nUpper);
#else
	ASSERT(0 <= nLower && nLower <= 65535);
	ASSERT(0 <= nUpper && nUpper <= 65535);
	CProgressCtrl::SetRange(nLower, nUpper);
#endif
	
	CProgressCtrl::SetStep(nStep);
	return TRUE;
}

int CProgressBar::SetPos(int nPos)	   
{
	if (!IsWindow(GetSafeHwnd()))
		return 0;

#ifdef PBM_SETRANGE32
	ASSERT(-0x7FFFFFFF <= nPos && nPos <= 0x7FFFFFFF);
#else
	ASSERT(0 <= nPos && nPos <= 65535);
#endif

	ModifyStyle(0,WS_VISIBLE);
	return CProgressCtrl::SetPos(nPos);
}

int CProgressBar::OffsetPos(int nPos) 
{ 
	if (!IsWindow(GetSafeHwnd()))
		return 0;

	ModifyStyle(0,WS_VISIBLE);
	return CProgressCtrl::OffsetPos(nPos);
}

int CProgressBar::SetStep(int nStep)
{ 
	if (!IsWindow(GetSafeHwnd()))
		return 0;

	ModifyStyle(0,WS_VISIBLE);
	return CProgressCtrl::SetStep(nStep);	 
}

int CProgressBar::StepIt() 			
{ 
	if (!IsWindow(GetSafeHwnd()))
		return 0;

	ModifyStyle(0,WS_VISIBLE);
	return CProgressCtrl::StepIt();    
}

BOOL CProgressBar::Resize() 
{
	if (!IsWindow(GetSafeHwnd()))
		return FALSE;

	CStatusBar *pStatusBar = GetStatusBar();
	if (!pStatusBar)
		return FALSE;

	// Redraw the window text
	if (IsWindowVisible())
	{
		pStatusBar->SetPaneText(m_nPane, m_strMessage);
		pStatusBar->UpdateWindow();
	}

	// Calculate how much space the text takes up
	CClientDC dc(pStatusBar);
	CFont *pOldFont = dc.SelectObject(pStatusBar->GetFont());
	CSize size = dc.GetTextExtent(m_strMessage);		// Length of text
	int margin = dc.GetTextExtent(_T(" ")).cx * 2;		// Text margin
	dc.SelectObject(pOldFont);

	// Now calculate the rectangle in which we will draw the progress bar
	CRect rc;
	pStatusBar->GetItemRect(m_nPane, rc);

	// Position left of progress bar after text and right of progress bar
	// to requested percentage of status bar pane
	if (!m_strMessage.IsEmpty())
	    rc.left += (size.cx + 2*margin);
	rc.right -= (rc.right - rc.left) * (100 - m_nSize) / 100;

	if (rc.right < rc.left) rc.right = rc.left;
	
	// Leave a litle vertical margin (10%) between the top and bottom of the bar
	int Height = rc.bottom - rc.top;
	rc.bottom -= Height/10;
	rc.top	  += Height/10;

	// If the window size has changed, resize the window
	if (rc != m_Rect)
	{
		MoveWindow(&rc);
		m_Rect = rc;
	}

	return TRUE;
}

BOOL CProgressBar::OnEraseBkgnd(CDC* pDC) 
{
	Resize();
	return CProgressCtrl::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////
BOOL PeekAndPump()
{
	static MSG msg;

	while (::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) {
		if (!AfxGetApp()->PumpMessage()) {
			::PostQuitMessage(0);
			return FALSE;
		}	
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

