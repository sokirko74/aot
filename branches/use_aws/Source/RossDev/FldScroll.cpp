// FldScroll.cpp : implementation file
//

#include "stdafx.h"
#include "FldScroll.h"
#include "ArticleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFldScroll

CFldScroll::CFldScroll()
{
	
}

CFldScroll::~CFldScroll()
{
}


BEGIN_MESSAGE_MAP(CFldScroll, CScrollBar)
	//{{AFX_MSG_MAP(CFldScroll)
	ON_WM_VSCROLL_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFldScroll message handlers

void CFldScroll::VScroll(UINT nSBCode, UINT nPos) 
{
//  CScrollBar:VScroll (nSBCode, nPos);

  // TODO: Add your message handler code here 
  if  (     (nSBCode == SB_LINEDOWN)   
	     || (nSBCode == SB_PAGEDOWN))   

  {
     SetScrollPos (GetScrollPos () + 1);
	 nPos = GetScrollPos();
  };

  if  (   (nSBCode == SB_LINEUP)
	    ||(nSBCode == SB_PAGEUP))   
  {
     SetScrollPos (GetScrollPos () - 1);
	 nPos = GetScrollPos();
  };


  if  (   (nSBCode == SB_THUMBTRACK) 
	   || (nSBCode == SB_LINEDOWN)   
   	   || (nSBCode == SB_PAGEDOWN)
	   || (nSBCode == SB_LINEUP)
	   || (nSBCode == SB_PAGEUP)
	   || (nSBCode == SB_THUMBPOSITION) )
  {
   CArticleView* V  =  (CArticleView*)GetParent();
   V->SetFocusToField (nPos);
   SetScrollPos (nPos);
  };

}

BOOL CFldScroll::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CScrollBar::PreCreateWindow(cs);
}
