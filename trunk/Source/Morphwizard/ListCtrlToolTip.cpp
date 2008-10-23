// ListCtrlToolTip.cpp : implementation file
//

#include "stdafx.h"
#include "Morphwizard.h"
#include "ListCtrlToolTip.h"


// CListCtrlToolTip

IMPLEMENT_DYNAMIC(CListCtrlToolTip, CListCtrl)
CListCtrlToolTip::CListCtrlToolTip()
{
	m_ToolTip.Create(this);
	m_strToolTipText = _T("");
	m_nPrevItem = -1;
	m_bMouseOver = FALSE;
	m_fnTTTextRetriever = NULL;

}

CListCtrlToolTip::~CListCtrlToolTip()
{
}




BEGIN_MESSAGE_MAP(CListCtrlToolTip, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlToolTip)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE (WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()



void CListCtrlToolTip::OnMouseMove(UINT nFlags, CPoint point) 
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


	CListCtrl::OnMouseMove(nFlags, point);
}


void CListCtrlToolTip::ShowToolTip(CPoint point)
{
	m_ToolTip.Close();
	point.x += 15;
	point.y += 10;

	m_ToolTip.SetText(m_strToolTipText);	
	m_ToolTip.Show(point);
	
}

void CListCtrlToolTip::SetToolTipText(LPCTSTR string, CPoint point)
{
	m_strToolTipText = string;

	ShowToolTip(point);
}

LRESULT CListCtrlToolTip::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    m_bMouseOver = FALSE;
	m_nPrevItem = -1;

	m_ToolTip.Close();

    return 0;
}
int CListCtrlToolTip::ItemFromPoint(CPoint point)
{
  //	 Get indexes of the first and last visible items in 
   // the listview control.
   int index = GetTopIndex();
   int last_visible_index = index + GetCountPerPage();
   if (last_visible_index > GetItemCount())
       last_visible_index = GetItemCount();

   // Loop until number visible items has been reached.
   while (index <= last_visible_index)
   {
       // Get the bounding rectangle of an item. If the mouse
       // location is within the bounding rectangle of the item,
       // you know you have found the item that was being clicked.
       CRect r;
       GetItemRect(index, &r, LVIR_BOUNDS);
       if (r.PtInRect(point))
               return index;
       // Get the next item in listview control.
       index++;
   }
   return -1;	
};

LRESULT CListCtrlToolTip::OnMouseHover(WPARAM wParam, LPARAM lParam)
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
	int		nItem = ItemFromPoint(point);
	if (nItem == -1) return 0;
	if ( nItem < m_ToolTips.size() )
			SetToolTipText(m_ToolTips[nItem].c_str(), point);

    return 0;
}


void CListCtrlToolTip::clear()
{
	DeleteAllItems();
	m_ToolTips.clear();
};
