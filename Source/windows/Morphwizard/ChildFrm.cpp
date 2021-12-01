// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "Morphwizard.h"

#include "ChildFrm.h"

#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#endif



// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
END_MESSAGE_MAP()

// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
//	cs.style |= WS_MAXIMIZE;	 // Nick [12/Dec/2003]
//	cs.style &= ~WS_MAXIMIZEBOX; // Nick [12/Dec/2003]
//	cs.style &= ~WS_MINIMIZEBOX; // Nick [12/Dec/2003]

	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


// CChildFrame diagnostics

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


// CChildFrame message handlers


