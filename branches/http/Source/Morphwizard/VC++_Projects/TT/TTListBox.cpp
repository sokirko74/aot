// ttlistbox.cpp : implementation file
//


#include "stdafx.h"
#include "ttlistbox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTTListBox

CTTListBox::CTTListBox()
{
	m_ToolTip.Create(this);
	m_strToolTipText = _T("");
	m_nPrevItem = -1;
	m_bMouseOver = FALSE;
	m_fnTTTextRetriever = NULL;
}

CTTListBox::~CTTListBox()
{
}


BEGIN_MESSAGE_MAP(CTTListBox, CListBox)
	//{{AFX_MSG_MAP(CTTListBox)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE (WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTTListBox message handlers

void CTTListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Set the input focus so that the user does not have to click
	// inside the listbox to display the tool tip

    if (!m_bMouseOver)
	{
        m_bMouseOver = TRUE;

        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof (tme);
        tme.dwFlags = TME_HOVER | TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        //tme.dwHoverTime = HOVER_DEFAULT;
		tme.dwHoverTime = 0;
        _TrackMouseEvent (&tme);
	}


	CListBox::OnMouseMove(nFlags, point);
}


void CTTListBox::ShowToolTip()
{
	// This function will be called every time the tool tip text needs to be
	// updated or displayed.

	m_ToolTip.Close();

	CRect rect;
	GetClientRect(&rect);

	CPoint point_temp = rect.BottomRight();
	CPoint point = CPoint(point_temp.x, 0);

	m_ToolTip.SetText(m_strToolTipText);	
	m_ToolTip.Show(point);

}

void CTTListBox::SetToolTipText(LPCTSTR string)
{
	m_strToolTipText = string;

	ShowToolTip();
}

LRESULT CTTListBox::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    m_bMouseOver = FALSE;
	m_nPrevItem = -1;

	m_ToolTip.Close();

    return 0;
}

LRESULT CTTListBox::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof (tme);
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.hwndTrack = m_hWnd;
    //tme.dwHoverTime = HOVER_DEFAULT;
	tme.dwHoverTime = 0;
    _TrackMouseEvent (&tme);
	//
	// Get the nearest item in the list box to the mouse cursor
	//
	CPoint point;
	point.x = LOWORD (lParam);
	point.y	= HIWORD (lParam);

	BOOL	bOutSide = FALSE;
	int		nItem = ItemFromPoint(point, bOutSide);
	//
	// Display the tool tip text only if the previous and current
	// item are different.
	//
	if(m_nPrevItem != nItem)
	{
		m_nPrevItem = nItem;
		if(m_fnTTTextRetriever != NULL)
			SetToolTipText(m_fnTTTextRetriever(nItem));
	}

    return 0;
}

void CTTListBox::SetTextRetrieverFcn(CString (*fn)( int))
{
	m_fnTTTextRetriever = (*fn);
}