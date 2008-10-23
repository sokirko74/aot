/////////////////////////////////////////////////////////////////////////////
// MFC Grid Control header file
// Written by Chris Maunder (Chris.Maunder@cbr.clw.csiro.au)
// Copyright (c) 1998.
// Adopted by Alexey Sokirko

#include "stdafx.h"
#include "GridCtrl.h"
#include "FormulaEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SLOP 3      // When resizing columns/row, the cursor has to be within +/-3
                    //   pixels of the dividing line for resizing to be possible



IMPLEMENT_DYNCREATE(CGridCtrl, CWnd)


void AFXAPI DDX_GridControl(CDataExchange* pDX, int nIDC, CGridCtrl& rControl)
{
    if (rControl.GetSafeHwnd() == NULL)    // not subclassed yet
    {
        ASSERT(!pDX->m_bSaveAndValidate);

        HWND hWndCtrl = pDX->PrepareCtrl(nIDC);

        if (!rControl.SubclassWindow(hWndCtrl))
        {
            ASSERT(FALSE);      // possibly trying to subclass twice?
            AfxThrowNotSupportedException();
        }
#ifndef _AFX_NO_OCC_SUPPORT
        else
        {
            // If the control has reparented itself (e.g., invisible control),
            // make sure that the CWnd gets properly wired to its control site.
            if (pDX->m_pDlgWnd->GetSafeHwnd() != ::GetParent(rControl.GetSafeHwnd()))
                rControl.AttachControlSite(pDX->m_pDlgWnd);
        }
#endif //!_AFX_NO_OCC_SUPPORT

    }
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl

CGridCtrl::CGridCtrl(int nRows, int nCols, int nFixedRows, int nFixedCols)
{
    RegisterWindowClass();
    
    // Store the system colours in case they change. The gridctrl uses
    // these colours, and in OnSysColorChange we can check to see if 
    // the gridctrl colours have been changed from the system colours.
    // If they have, then leave them, otherwise change them to reflect
    // the new system colours.
    m_crWindowText       = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour     = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace           = ::GetSysColor(COLOR_3DFACE);
    m_crShadow           = ::GetSysColor(COLOR_3DSHADOW);

    m_nRows              = 0;
    m_nCols              = 0;
    m_nFixedRows         = 0;
    m_nFixedCols         = 0;

    m_nDefCellHeight     = 10;        // These will get changed to something meaningful
    m_nDefCellHeight     = 30;        //    when the window is created or subclassed

    m_nVScrollMax        = 0;         // Scroll position
    m_nHScrollMax        = 0;
    m_nMargin            = 0;         // cell padding

    m_MouseMode          = MOUSE_NOTHING;
    m_nGridLines         = GVL_BOTH;
    m_bEditable          = TRUE;
    m_bAllowDraw         = TRUE;      // allow draw updates
    m_bEnableSelection   = TRUE;
    m_bAllowRowResize    = TRUE;
    m_bAllowColumnResize = TRUE;


    m_nTimerID           = 0;         // For drag-selection
    m_nTimerInterval     = 25;        // (in milliseconds)

    m_pDropWnd           = NULL;

    // Initially use the system message font for the GridCtrl font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    m_Font.CreateFontIndirect(&(ncm.lfMessageFont));

    // Set up the initial grid size
    SetRowCount(nRows);
    SetColumnCount(nCols);
    SetFixedRowCount(nFixedRows);
    SetFixedColumnCount(nFixedCols);

    // Set the colours
    SetTextColor(m_crWindowText);
    SetTextBkColor(m_crWindowColour);
    SetBkColor(m_crShadow);
    SetFixedTextColor(m_crWindowText);
    SetFixedBkColor(m_cr3DFace);

    // set initial selection range (ie. none)
    m_SelectedCells.RemoveAll();
    m_PrevSelectedCells.RemoveAll();
    m_SelectedCells.SetSize(0,100);
}

CGridCtrl::~CGridCtrl()
{
    DeleteContents();

    if (m_Font.m_hObject) m_Font.DeleteObject();
    DestroyWindow();
}

// Register the window class if it has not already been registered.
BOOL CGridCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, GRIDCTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = NULL;
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = GRIDCTRL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls)) {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CGridCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
    ASSERT(pParentWnd->GetSafeHwnd());

    if (!CWnd::Create(GRIDCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID)) 
        return FALSE;

    OnSetFont( (LPARAM)((HFONT)m_Font), 0);
    
    // The number of rows and columns will only be non-zero if the constructor
    // was called with non-zero initialising parameters. If this window was created
    // using a dialog template then the number of rows and columns will be 0 (which
    // means that the code below will not be needed - which is lucky 'cause it ain't
    // gonna get called in a dialog-template-type-situation.

    try {
        m_arRowHeights.SetSize(m_nRows);    // initialize row heights
        m_arColWidths.SetSize(m_nCols);     // initialize column widths
    }
    catch (CMemoryException *e) {
        e->ReportError();
        e->Delete();
        return FALSE;
    }

    for (int i = 0; i < m_nRows; i++) m_arRowHeights[i] = m_nDefCellHeight;
    for (int i = 0; i < m_nCols; i++)      m_arColWidths[i] = m_nDefCellWidth;

    ResetScrollBars();
    return TRUE;
}

void CGridCtrl::PreSubclassWindow() 
{    
    CWnd::PreSubclassWindow();

    OnSetFont( (LPARAM)((HFONT)m_Font), 0);
    ResetScrollBars();   
}

BOOL CGridCtrl::SubclassWindow(HWND hWnd) 
{    
    if (!CWnd::SubclassWindow(hWnd)) return FALSE;

    OnSetFont( (LPARAM)((HFONT)m_Font), 0);
    ResetScrollBars();
    return TRUE;
}


BEGIN_MESSAGE_MAP(CGridCtrl, CWnd)
    //{{AFX_MSG_MAP(CGridCtrl)
    ON_WM_PAINT()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_CAPTURECHANGED()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_SETCURSOR()
    ON_WM_SETTINGCHANGE()
    ON_WM_SYSCOLORCHANGE()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_SETFONT, OnSetFont)
    ON_MESSAGE(WM_GETFONT, OnGetFont)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndInPlaceEdit)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl message handlers

void CGridCtrl::OnPaint() 
{
    CPaintDC dc(this); // device context for painting
    OnDraw(dc);
}

BOOL CGridCtrl::OnEraseBkgnd(CDC* /*pDC*/) 
{
    return TRUE;    // Don't erase the background.
}

// Custom background erasure. This gets called from within the OnDraw function,
// since we will (most likely) be using a memory DC to stop flicker. If we just
// erase the background normally through OnEraseBkgnd, and didn't fill the memDC's
// selected bitmap with colour, then all sorts of vis problems would occur
void CGridCtrl::EraseBkgnd(CDC* pDC) 
{
    CRect  VisRect, ClipRect, rect;

    if (pDC->GetClipBox(ClipRect) == ERROR) return;
    GetVisibleNonFixedCellRange(VisRect);

    // Draw Fixed columns background
    int nFixedColumnWidth = GetFixedColumnWidth();
    if (ClipRect.left < nFixedColumnWidth && ClipRect.top < VisRect.bottom)
        pDC->FillSolidRect(ClipRect.left, ClipRect.top,
                           nFixedColumnWidth - ClipRect.left+1, 
                           VisRect.bottom - ClipRect.top, GetFixedBkColor());
        
    // Draw Fixed rows background
    int nFixedRowHeight = GetFixedRowHeight();
    if (ClipRect.top < nFixedRowHeight && 
        ClipRect.right > nFixedColumnWidth && ClipRect.left < VisRect.right)
        pDC->FillSolidRect(nFixedColumnWidth-1, ClipRect.top,
                           VisRect.right - nFixedColumnWidth+1,
                           nFixedRowHeight - ClipRect.top, GetFixedBkColor());

    // Draw non-fixed cell background
    if (rect.IntersectRect(VisRect, ClipRect)) 
    {
        CRect CellRect(max(nFixedColumnWidth, rect.left), max(nFixedRowHeight, rect.top),
                       rect.right, rect.bottom);
        pDC->FillSolidRect(CellRect, GetTextBkColor());
    }

    // Draw right hand side of window outside grid
    if (VisRect.right < ClipRect.right) 
        pDC->FillSolidRect(VisRect.right, ClipRect.top, 
                           ClipRect.right - VisRect.right+1, ClipRect.Height(), 
                           GetBkColor());

    // Draw bottom of window below grid
    if (VisRect.bottom < ClipRect.bottom && ClipRect.left < VisRect.right) 
        pDC->FillSolidRect(ClipRect.left, VisRect.bottom, 
                           VisRect.right-ClipRect.left+1, ClipRect.bottom - VisRect.bottom+1, 
                           GetBkColor());
}

void CGridCtrl::OnSize(UINT nType, int cx, int cy) 
{
//    if (::IsWindow(GetSafeHwnd()) && GetFocus()->GetSafeHwnd() != GetSafeHwnd()) 
        SetFocus();        // Auto-destroy any InPlaceEdit's

    CWnd::OnSize(nType, cx, cy);
    ResetScrollBars();    
}

UINT CGridCtrl::OnGetDlgCode() 
{
    return DLGC_WANTARROWS | DLGC_WANTCHARS;
}


// For drag-selection. Scrolls hidden cells into view
// TODO: decrease timer interval over time to speed up selection over time
void CGridCtrl::OnTimer(UINT nIDEvent)
{
    ASSERT(nIDEvent == WM_LBUTTONDOWN);
    CPoint pt, origPt;

    if (!GetCursorPos(&origPt)) return;
    ScreenToClient(&origPt);

    CRect rect;
    GetClientRect(rect);

    int nFixedRowHeight = GetFixedRowHeight();
    int nFixedColWidth = GetFixedColumnWidth();

    pt = origPt;
    if (pt.y > rect.bottom)
    {
        //SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
        SendMessage(WM_KEYDOWN, VK_DOWN, 0);

        if (pt.x < rect.left)  pt.x = rect.left;
        if (pt.x > rect.right) pt.x = rect.right;
        pt.y = rect.bottom;
        OnSelecting(GetCellFromPt(pt));
    }
    else if (pt.y < nFixedRowHeight)
    {
        //SendMessage(WM_VSCROLL, SB_LINEUP, 0);
        SendMessage(WM_KEYDOWN, VK_UP, 0);

        if (pt.x < rect.left)  pt.x = rect.left;
        if (pt.x > rect.right) pt.x = rect.right;
        pt.y = nFixedRowHeight + 1;
        OnSelecting(GetCellFromPt(pt));
    }

    pt = origPt;
    if (pt.x > rect.right)
    {
        // SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
        SendMessage(WM_KEYDOWN, VK_RIGHT, 0);

        if (pt.y < rect.top)    pt.y = rect.top;
        if (pt.y > rect.bottom) pt.y = rect.bottom;
        pt.x = rect.right;
        OnSelecting(GetCellFromPt(pt));
    } 
    else if (pt.x < nFixedColWidth)
    {
        //SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
        SendMessage(WM_KEYDOWN, VK_LEFT, 0);

        if (pt.y < rect.top)    pt.y = rect.top;
        if (pt.y > rect.bottom) pt.y = rect.bottom;
        pt.x = nFixedColWidth + 1;
        OnSelecting(GetCellFromPt(pt));
    }

}

// move about with keyboard
void CGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if (!IsValid(m_idCurrentCell)) return;

    CCellID next = m_idCurrentCell;

    switch (nChar)
    {
        case VK_DOWN:  if (next.row < (GetRowCount() - 1)) next.row++; break;
        case VK_UP:    if (next.row > m_nFixedRows)        next.row--; break;
        case VK_RIGHT: if (next.col < (GetColumnCount() - 1)) next.col++; break;
        case VK_LEFT:  if (next.col > m_nFixedCols)        next.col--; break;

        case VK_NEXT:  {
                          CCellID idOldTopLeft = GetTopleftNonFixedCell();
                          SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
                          CCellID idNewTopLeft = GetTopleftNonFixedCell();

                          int increment = idNewTopLeft.row - idOldTopLeft.row;
                          if (increment) {
                              next.row += increment;
                              if (next.row > (GetRowCount() - 1)) 
                                  next.row = GetRowCount() - 1;
                          }
                          else
                              next.row = GetRowCount() - 1;
                          break;
                       }

        case VK_PRIOR: {
                          CCellID idOldTopLeft = GetTopleftNonFixedCell();
                          SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
                          CCellID idNewTopLeft = GetTopleftNonFixedCell();

                          int increment = idNewTopLeft.row - idOldTopLeft.row;
                          if (increment) {
                              next.row += increment;
                              if (next.row < m_nFixedRows) next.row = m_nFixedRows;
                          } else
                              next.row = m_nFixedRows;
                          break;
                       }

        case VK_HOME:  SendMessage(WM_VSCROLL, SB_TOP, 0);
                       next.row = m_nFixedRows;
                       break;
    
        case VK_END:   {
                          SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
                          next.row = GetRowCount() - 1;
                          break;
                       }
        default:
            CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
    }

    if (next != m_idCurrentCell) 
    {
        SetFocusCell(next);
        while (!IsCellVisible(next))
        {
            switch (nChar) {
                case VK_RIGHT: SendMessage(WM_HSCROLL, SB_LINERIGHT, 0); break;
                case VK_LEFT:  SendMessage(WM_HSCROLL, SB_LINELEFT, 0);  break;
                case VK_DOWN:  SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);  break;
                case VK_UP:    SendMessage(WM_VSCROLL, SB_LINEUP, 0);    break;
            }
            Invalidate();
        }
    }
}

// Instant editing of cells when keys are pressed
void CGridCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if (m_MouseMode == MOUSE_NOTHING && IsCellVisible(m_idCurrentCell))
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, nChar);

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

// Callback from any CInPlaceEdits that ended. This just calls OnEndEditCell,
// refreshes the edited cell and moves onto next cell if the return character
// from the edit says we should.
void CGridCtrl::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{    
    LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
    LV_ITEM    *plvItem = &plvDispInfo->item;

    OnEndEditCell(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
    InvalidateCellRect(CCellID(plvItem->iItem, plvItem->iSubItem));

    switch (plvItem->lParam) 
    {
        case VK_DOWN: 
        case VK_UP:   
        case VK_RIGHT:
        case VK_LEFT:  
        case VK_NEXT:  
        case VK_PRIOR: 
        case VK_HOME:  
        case VK_END:    OnKeyDown(plvItem->lParam, 0, 0);
                        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, plvItem->lParam);
    }

    if (plvItem->lParam != VK_ESCAPE) SetModified(TRUE);

    *pResult = 0;
}

// Handle horz scrollbar notifications
void CGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
    if (GetFocus()->GetSafeHwnd() != GetSafeHwnd()) SetFocus();  // Auto-destroy any InPlaceEdit's

    int scrollPos = GetScrollPos(SB_HORZ);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rect;
    GetClientRect(rect);

    switch (nSBCode)
    {
        case SB_LINERIGHT:
            if (scrollPos < m_nHScrollMax)
            {
                int xScroll = GetColumnWidth(idTopLeft.col);
                SetScrollPos(SB_HORZ, scrollPos + xScroll);
                if (GetScrollPos(SB_HORZ) == scrollPos) break;            // didn't work

                rect.left = GetFixedColumnWidth() + xScroll;
                ScrollWindow(-xScroll, 0, rect);
                rect.left = rect.right - xScroll;
                InvalidateRect(rect);
            }
            break;

        case SB_LINELEFT:
            if (scrollPos > 0 && idTopLeft.col > GetFixedColumnCount())
            {
                int xScroll = GetColumnWidth(idTopLeft.col-1);
                SetScrollPos(SB_HORZ, max(0,scrollPos - xScroll));
                rect.left = GetFixedColumnWidth();
                ScrollWindow(xScroll, 0, rect);
                rect.right = rect.left + xScroll;
                InvalidateRect(rect);
            }
            break;

        case SB_PAGERIGHT:
            if (scrollPos < m_nHScrollMax)
            {
                rect.left = GetFixedColumnWidth();
                int offset = rect.Width();
                int pos = min(m_nHScrollMax, scrollPos + offset);
                SetScrollPos(SB_HORZ, pos);
                rect.left = GetFixedColumnWidth();
                InvalidateRect(rect);
            }
            break;

        case SB_PAGELEFT:
            if (scrollPos > 0)
            {
                rect.left = GetFixedColumnWidth();
                int offset = -rect.Width();
                int pos = max(0, scrollPos + offset);
                SetScrollPos(SB_HORZ, pos);
                rect.left = GetFixedColumnWidth();
                InvalidateRect(rect);
            }
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            {
                SetScrollPos(SB_HORZ, nPos);
                rect.left = GetFixedColumnWidth();
                InvalidateRect(rect);
            }
            break;

        case SB_LEFT:
            if (scrollPos > 0)
            {
                SetScrollPos(SB_HORZ, 0);
                Invalidate();
            }
            break;

        case SB_RIGHT:
            if (scrollPos < m_nHScrollMax)
            {
                SetScrollPos(SB_HORZ, m_nHScrollMax);
                Invalidate();
            }
            break;


        default: break;
    }
}

// Handle vert scrollbar notifications
void CGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
    if (GetFocus()->GetSafeHwnd() != GetSafeHwnd()) SetFocus();        // Auto-destroy any InPlaceEdit's

    int scrollPos = GetScrollPos(SB_VERT);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rect;
    GetClientRect(rect);

    switch (nSBCode)
    {
        case SB_LINEDOWN:
            if (scrollPos < m_nVScrollMax)
            {
                int yScroll = GetRowHeight(idTopLeft.row);
                SetScrollPos(SB_VERT, scrollPos + yScroll);
                if (GetScrollPos(SB_VERT) == scrollPos) break;            // didn't work

                rect.top = GetFixedRowHeight() + yScroll;
                ScrollWindow( 0, -yScroll, rect);
                rect.top = rect.bottom - yScroll;
                InvalidateRect(rect);
            }
            break;

        case SB_LINEUP:
            if (scrollPos > 0 && idTopLeft.row > GetFixedRowCount())
            {
                int yScroll = GetRowHeight(idTopLeft.row-1);
                SetScrollPos(SB_VERT, max(0, scrollPos - yScroll));
                rect.top = GetFixedRowHeight();
                ScrollWindow(0, yScroll, rect);
                rect.bottom = rect.top + yScroll;
                InvalidateRect(rect);
            }
            break;

        case SB_PAGEDOWN:
            if (scrollPos < m_nVScrollMax)
            {
                rect.top = GetFixedRowHeight();
                scrollPos = min(m_nVScrollMax, scrollPos + rect.Height());
                SetScrollPos(SB_VERT, scrollPos);
                rect.top = GetFixedRowHeight();
                InvalidateRect(rect);
            }
            break;

        case SB_PAGEUP:
            if (scrollPos > 0)
            {
                rect.top = GetFixedRowHeight();
                int offset = -rect.Height();
                int pos = max(0, scrollPos + offset);
                SetScrollPos(SB_VERT, pos);
                rect.top = GetFixedRowHeight();
                InvalidateRect(rect);
            }
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            {
                SetScrollPos(SB_VERT, nPos);
                rect.top = GetFixedRowHeight();
                InvalidateRect(rect);
            }
            break;

        case SB_TOP:
            if (scrollPos > 0)
            {
                SetScrollPos(SB_VERT, 0);
                Invalidate();
            }
            break;

        case SB_BOTTOM:
            if (scrollPos < m_nVScrollMax)
            {
                SetScrollPos(SB_VERT, m_nVScrollMax);
                Invalidate();
            }

        default: break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl implementation functions

void CGridCtrl::OnDraw(CDC& dc)
{
    CRect rect;
    int row,col;

    CRect clipRect;
    if (dc.GetClipBox(&clipRect) == ERROR) return;

    EraseBkgnd(&dc);            // OnEraseBkgnd does nothing, so erase bkgnd here.
                                // This necessary since we may be using a Memory DC.

    int nFixedRowHeight = GetFixedRowHeight();
    int nFixedColWidth  = GetFixedColumnWidth();

    CCellID idTopLeft = GetTopleftNonFixedCell();
    int minVisibleRow = idTopLeft.row,
        minVisibleCol = idTopLeft.col;

    CRect VisRect;
    CCellRange VisCellRange = GetVisibleNonFixedCellRange(VisRect);
    int maxVisibleRow = VisCellRange.GetMaxRow(),
        maxVisibleCol = VisCellRange.GetMaxCol();

    // draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;
        rect.right = -1;
        for (col = 0; col < m_nFixedCols; col++)
        {
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;
            DrawFixedCell(&dc, row, col, rect);
        }
    }
     
    // draw fixed column cells:  m_nFixedRows..n, 0..m_nFixedCols-1
    rect.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom) break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)    continue;          // Reached cliprect yet?

        rect.right = -1;
        for (col = 0; col < m_nFixedCols; col++)
        {
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right) break;          // gone past cliprect
            if (rect.right < clipRect.left)    continue;    // Reached cliprect yet?

            DrawFixedCell(&dc, row, col, rect);
        }
    }
    
    // draw fixed row cells  0..m_nFixedRows, m_nFixedCols..n
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom) break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)    continue;          // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {                                       
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right) break;        // gone past cliprect
            if (rect.right < clipRect.left) continue;     // Reached cliprect yet?

            DrawFixedCell(&dc, row, col, rect);
        }
    }
    
    // draw rest of non-fixed cells
    rect.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom) break;                // Gone past cliprect
        if (rect.bottom < clipRect.top) continue;             // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right) break;        // gone past cliprect
            if (rect.right < clipRect.left) continue;     // Reached cliprect yet?

            DrawCell(&dc, row, col, rect);
        }
    }

    // draw vertical lines (drawn at ends of cells)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT) 
    {
        int x = nFixedColWidth;
        for (col = minVisibleCol; col <= maxVisibleCol; col++) {
            x += GetColumnWidth(col);
            dc.MoveTo(x-1, nFixedRowHeight);
            dc.LineTo(x-1, VisRect.bottom);   
        }
    }
    
    // draw horizontal lines (drawn at bottom of each cell)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ) 
    {
        int y = nFixedRowHeight;
        for (row = minVisibleRow; row <= maxVisibleRow; row++) {
            y += GetRowHeight(row);
            dc.MoveTo(nFixedColWidth, y-1);    
            dc.LineTo(VisRect.right,  y-1);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// CGridCtrl Cell selection stuff

BOOL CGridCtrl::IsValid(int nRow, int nCol)
{
    return (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols);
}

BOOL CGridCtrl::IsValid(const CCellID& cell)
{
    return IsValid(cell.row, cell.col);
}

BOOL CGridCtrl::IsValid(const CCellRange& range)
{
     return (range.GetMinRow() >= 0 && range.GetMinCol() >= 0 && 
            range.GetMaxRow() >= 0 && range.GetMaxCol() >= 0 &&
            range.GetMaxRow() < m_nRows && range.GetMaxCol() < m_nCols &&
             range.GetMinRow() <= range.GetMaxRow() && range.GetMinCol() <= range.GetMaxCol());
}

// Enables/Disables redraw for certain operations like columns auto-sizing etc,
// but not for user caused things such as selection changes.
void CGridCtrl::SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars /* = FALSE */)
{
    if (bAllowDraw && !m_bAllowDraw) Invalidate();
    m_bAllowDraw = bAllowDraw;
    if (bResetScrollBars) ResetScrollBars();
}

// Forces a redraw of a cell immediately (using a direct DC construction, 
// or the supplied dc)
BOOL CGridCtrl::RedrawCell(const CCellID& cell, CDC* pDC /* = NULL */)
{
    return RedrawCell(cell.row, cell.col, pDC);
}

BOOL CGridCtrl::RedrawCell(int nRow, int nCol, CDC* pDC /* = NULL */)
{
    BOOL bResult = TRUE;
    BOOL bMustReleaseDC = FALSE;

    if (!m_bAllowDraw || !IsCellVisible(nRow, nCol)) return FALSE;

    CRect rect;
    if (!GetCellRect(nRow, nCol, rect)) return FALSE;

    if (!pDC) {
        pDC = GetDC();
        if (pDC) bMustReleaseDC = TRUE;
    }

    if (pDC)
    {
        // Redraw cells directly
        if (nRow < m_nFixedRows || nCol < m_nFixedCols)
            bResult = DrawFixedCell(pDC, nRow, nCol, rect, TRUE);
        else
            bResult = DrawCell(pDC, nRow, nCol, rect, TRUE);

        // Since we have erased the background, we will need to redraw the gridlines
        pDC->SelectStockObject(BLACK_PEN);
        
        if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ) 
        {
            pDC->MoveTo(rect.left,    rect.bottom);
            pDC->LineTo(rect.right+1, rect.bottom);
        }
        if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT) 
        {
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.right, rect.bottom+1);    
        }
    } else
        InvalidateRect(rect, TRUE);     // Could not get a DC - invalidate it anyway
                                        // and hope that OnPaint manages to get one

    if (bMustReleaseDC) ReleaseDC(pDC);

    return bResult;
}

// redraw a complete row
BOOL CGridCtrl::RedrawRow(int row)
{
    BOOL bResult = TRUE;

    CDC* pDC = GetDC();
    for (int col = 0; col < GetColumnCount(); col++)
        bResult = bResult && RedrawCell(row, col, pDC);
    if (pDC) ReleaseDC(pDC);

    return bResult;
}

// redraw a complete column
BOOL CGridCtrl::RedrawColumn(int col)
{
    BOOL bResult = TRUE;

    CDC* pDC = GetDC();
    for (int row = 0; row < GetRowCount(); row++)
        bResult = bResult && RedrawCell(row, col, pDC);
    if (pDC) ReleaseDC(pDC);

    return bResult;
}


// Sets the currently selected cell, returning the previous current cell
CCellID CGridCtrl::SetFocusCell(int nRow, int nCol)
{
    return SetFocusCell(CCellID(nRow, nCol));
}

CCellID CGridCtrl::SetFocusCell(CCellID cell)
{
    if (cell == m_idCurrentCell) return m_idCurrentCell;

    CCellID idPrev = m_idCurrentCell;
    m_idCurrentCell = cell;

    if (IsValid(idPrev)) {
        SetItemState(idPrev.row, idPrev.col, 
                     GetItemState(idPrev.row, idPrev.col) & ~GVIS_FOCUSED);
        RedrawCell(idPrev);

        if (idPrev.col != m_idCurrentCell.col)
            for (int row = 0; row < m_nFixedRows; row++) RedrawCell(row, idPrev.col);
        if (idPrev.row != m_idCurrentCell.row)
            for (int col = 0; col < m_nFixedCols; col++) RedrawCell(idPrev.row, col);
    }

    if (IsValid(m_idCurrentCell)) {
        SetItemState(m_idCurrentCell.row, m_idCurrentCell.col, 
                     GetItemState(m_idCurrentCell.row, m_idCurrentCell.col) | GVIS_FOCUSED);

        RedrawCell(m_idCurrentCell);

        if (idPrev.col != m_idCurrentCell.col)
            for (int row = 0; row < m_nFixedRows; row++) RedrawCell(row, m_idCurrentCell.col);
        if (idPrev.row != m_idCurrentCell.row)
            for (int col = 0; col < m_nFixedCols; col++) RedrawCell(m_idCurrentCell.row, col);
    }

    return idPrev;
}

// Sets the range of currently selected cells
void CGridCtrl::SetSelectedRange(const CCellRange& Range, 
                                 BOOL bForceRepaint /* = FALSE */)
{
    SetSelectedRange(Range.GetMinRow(), Range.GetMinCol(), Range.GetMaxRow(), Range.GetMaxCol(),
                     bForceRepaint);
}

void CGridCtrl::SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
                                 BOOL bForceRepaint /* = FALSE */)
{
    if (!m_bEnableSelection) return;

    CDC* pDC = NULL;
    if (bForceRepaint) pDC = GetDC();

    // Unselect all previously selected cells
    // TODO - This is kinda slow. A quicker way is to set the state of each cell
    //        as GetItemState(cell.row, cell.col) & ~GVIS_SELECTED directly and
    //        then just call m_SelectedCells.RemoveAll() at the end of the loop.

    for (int i = m_SelectedCells.GetSize()-1; i >= 0; i--)
    {
        CCellID cell = m_SelectedCells[i];

        // Reset the selection flag on the cell
        if (IsValid(cell)) {
            SetItemState(cell.row, cell.col, 
                         GetItemState(cell.row, cell.col) & ~GVIS_SELECTED);

            // If this is to be reselected, continue on past the redraw
            if (nMinRow <= cell.row && cell.row <= nMaxRow &&
                nMinCol <= cell.col && cell.col <= nMaxCol)
                    continue;

            if (bForceRepaint && pDC)                    // Redraw NOW
                RedrawCell(cell.row, cell.col, pDC);
            else
                InvalidateCellRect(cell);                // Redraw at leisure
        }
    }
    
    // if any previous selected cells are to be retained (eg Ctrl is being held down)
    // then copy them to the newly created list, and mark all these cells as
    // selected
    for (int i = 0; i < m_PrevSelectedCells.GetSize(); i++)
    {
        CCellID cell = m_PrevSelectedCells[i];

        if (!IsValid(cell)) continue;

        int nState = GetItemState(cell.row, cell.col);

        // Set state as Selected. This will add the cell to m_SelectedCells[]
        SetItemState(cell.row, cell.col, nState | GVIS_SELECTED);

        // Redraw (immediately or at leisure)
        if (bForceRepaint && pDC)
            RedrawCell(cell.row, cell.col, pDC);
        else
            InvalidateCellRect(cell);
    }

    // Now select all cells in the cell range specified. If the cell has already
    // been marked as selected (above) then ignore it.
    if (nMinRow >= 0 && nMinCol >= 0 && nMaxRow >= 0 && nMaxCol >= 0 &&
        nMaxRow < m_nRows && nMaxCol < m_nCols &&
         nMinRow <= nMaxRow && nMinCol <= nMaxCol)
    {
        for (int row = nMinRow; row <= nMaxRow; row++)
            for (int col = nMinCol; col <= nMaxCol; col++) 
            {
                int nState = GetItemState(row, col);
                if (nState & GVIS_SELECTED) {
                    continue;    // Already selected - ignore
                }

                // Add to list of selected cells
                CCellID cell(row, col);

                // Set state as Selected. This will add the cell to m_SelectedCells[]
                SetItemState(row, col, nState | GVIS_SELECTED);

                // Redraw (immediately or at leisure)
                if (bForceRepaint && pDC)
                    RedrawCell(row, col, pDC);
                else
                    InvalidateCellRect(cell);
            }
    }
    TRACE("%d cells selected.\n", m_SelectedCells.GetSize());

    if (pDC != NULL) ReleaseDC(pDC);
} 



// selects cells
void CGridCtrl::SelectCells(CCellID currentCell)
{
    if (!m_bEnableSelection) return;

    int row = currentCell.row;
    int col = currentCell.col;
    if (row < m_nFixedRows || col < m_nFixedCols) return;
    if (!IsValid(currentCell)) return;

    // Prevent unnecessary redraws
    //if (currentCell == m_LeftClickDownCell)  return;
    //else if (currentCell == m_idCurrentCell) return;

    SetSelectedRange(min(m_SelectionStartCell.row, row), min(m_SelectionStartCell.col, col), 
                     max(m_SelectionStartCell.row, row), max(m_SelectionStartCell.col, col)); 
}

void CGridCtrl::OnSelecting(const CCellID& currentCell)
{
    if (!m_bEnableSelection) return;

    switch(m_MouseMode)
    {
        case MOUSE_SELECT_CELLS: SelectCells(currentCell);         break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//

// Get cell from point
CCellID CGridCtrl::GetCellFromPt(CPoint point)
{  
    CCellID idTopLeft = GetTopleftNonFixedCell();
    CCellID cellID; // return value

    // calculate column index
    int fixedColWidth = GetFixedColumnWidth();

    if (point.x < 0) // not in window
        cellID.col = -1;
    else if (point.x < fixedColWidth) // in fixed col
    {
        int xpos = 0;
		int col = 0;
        for (; col < m_nFixedCols; col++)
        {
            xpos += GetColumnWidth(col);
            if (xpos > point.x) break;
        }
        cellID.col = col;
    }
    else    // in non-fixed col
    {
        int xpos = fixedColWidth;
		int col = idTopLeft.col;
        for (; col < GetColumnCount(); col++)
        {
            xpos += GetColumnWidth(col);
            if (xpos > point.x) break;
        }

        if (col >= GetColumnCount())
            cellID.col = GetColumnCount() - 1;
        else
            cellID.col = col;
    }
    
    // calculate row index
    int fixedRowHeight = GetFixedRowHeight();
    if (point.y < 0) // not in window
        cellID.row = -1;
    else if (point.y < fixedRowHeight) // in fixed col
    {
        int ypos = 0;
		int row = 0;
        for (; row < m_nFixedRows; row++)
        {
            ypos += GetRowHeight(row);
            if (ypos > point.y) break;
        }
        cellID.row = row;
    }
    else
    {
        int ypos = fixedRowHeight;
		int row = idTopLeft.row;
        for (; row < GetRowCount(); row++)
        {
            ypos += GetRowHeight(row);
            if (ypos > point.y) break;
        }

        if (row >= GetRowCount())
            cellID.row = -1;
        else
            cellID.row = row;
    }

    return cellID;
}

////////////////////////////////////////////////////////////////////////////////
// CGridCtrl cellrange functions

CCellID CGridCtrl::GetTopleftNonFixedCell()
{
    int nVertScroll = GetScrollPos(SB_VERT), 
        nHorzScroll = GetScrollPos(SB_HORZ);

    int nColumn = m_nFixedCols, nRight = 0;
    while (nRight < nHorzScroll && nColumn < (GetColumnCount()-1))
        nRight += GetColumnWidth(nColumn++);

    int nRow = m_nFixedRows, nTop = 0;
    while (nTop < nVertScroll && nRow < (GetRowCount()-1))
        nTop += GetRowHeight(nRow++);

    //TRACE("TopLeft cell is row %d, col %d\n",nRow, nColumn);
    return CCellID(nRow, nColumn);
}

// This gets even partially visible cells
CCellRange CGridCtrl::GetVisibleNonFixedCellRange(LPRECT pRect /*=NULL*/)
{
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    // calc bottom
    int bottom = GetFixedRowHeight();
	int i = idTopLeft.row;
    for (; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom) {
            bottom = rect.bottom;
            break;
        }
    }                                
    int maxVisibleRow = min(i, GetRowCount() - 1);
    
    // calc right
    int right = GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right) {
            right = rect.right;
            break;
        }
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
    if (pRect) {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}

// used by ResetScrollBars() - This gets only fully visible cells
CCellRange CGridCtrl::GetUnobstructedNonFixedCellRange()
{
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    // calc bottom
    int bottom = GetFixedRowHeight();
	int i = idTopLeft.row;
    for (; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom) break;
    }
    int maxVisibleRow = min(i, GetRowCount() - 1);
    if (maxVisibleRow > 0 && bottom > rect.bottom) maxVisibleRow--;
    
    // calc right
    int right = GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right) break;
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
    if (maxVisibleCol > 0 && right > rect.right) maxVisibleCol--;


    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}

// Returns ALL the cells in the grid
CCellRange CGridCtrl::GetCellRange()
{
    return CCellRange(0, 0, GetRowCount() - 1, GetColumnCount() - 1);
}

void CGridCtrl::ResetSelectedRange()
{
    SetSelectedRange(-1,-1,-1,-1);
}

void CGridCtrl::ResetScrollBars()
{
    if (!m_bAllowDraw || !::IsWindow(GetSafeHwnd())) return;

    CRect rect;
    GetClientRect(rect);
    rect.left  += GetFixedColumnWidth();
    rect.top   += GetFixedRowHeight();
    if (rect.left >= rect.right || rect.top >= rect.bottom) return;

    CRect VisibleRect(GetFixedColumnWidth(), GetFixedRowHeight(), rect.right, rect.bottom);
    CRect VirtualRect(GetFixedColumnWidth(), GetFixedRowHeight(), GetVirtualWidth(), GetVirtualHeight());

    CCellRange visibleCells = GetUnobstructedNonFixedCellRange();
    if (!IsValid(visibleCells)) return;

    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_PAGE;
    si.nPage  = VisibleRect.Width();    SetScrollInfo(SB_HORZ, &si, FALSE); 
    si.nPage  = VisibleRect.Height();    SetScrollInfo(SB_VERT, &si, FALSE); 

    if (VisibleRect.Height() < VirtualRect.Height())
        m_nVScrollMax = VirtualRect.Height()-1; //+ GetRowHeight(visibleCells.GetTopLeft().row);
    else
        m_nVScrollMax = 0;

    if (VisibleRect.Width() < VirtualRect.Width())
        m_nHScrollMax = VirtualRect.Width()-1; //+ GetColumnWidth(visibleCells.GetTopLeft().col);
    else
        m_nHScrollMax = 0;

    ASSERT(m_nVScrollMax < INT_MAX && m_nHScrollMax < INT_MAX); // This should be fine :)
    SetScrollRange(SB_VERT, 0, m_nVScrollMax, TRUE);
    SetScrollRange(SB_HORZ, 0, m_nHScrollMax, TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column position functions

// returns the top left point of the cell. Returns FALSE if cell not visible.
BOOL CGridCtrl::GetCellOrigin(int nRow, int nCol, LPPOINT p)
{
    int i;

    if (!IsValid(nRow, nCol)) return FALSE;

    CCellID idTopLeft;
    if (nCol >= m_nFixedCols || nRow >= m_nFixedRows)
        idTopLeft = GetTopleftNonFixedCell();

    if ((nRow >= m_nFixedRows && nRow < idTopLeft.row) ||
        (nCol>= m_nFixedCols && nCol < idTopLeft.col))
        return FALSE;

    p->x = 0;
    if (nCol < m_nFixedCols)                      // is a fixed column
        for (i = 0; i < nCol; i++)
            p->x += GetColumnWidth(i);
    else {                                        // is a scrollable data column
        for (i = 0; i < m_nFixedCols; i++)
            p->x += GetColumnWidth(i);
        for (i = idTopLeft.col; i < nCol; i++)
            p->x += GetColumnWidth(i);
    }

    p->y = 0;
    if (nRow < m_nFixedRows)                      // is a fixed row
        for (i = 0; i < nRow; i++)
            p->y += GetRowHeight(i);
    else {                                        // is a scrollable data row
        for (i = 0; i < m_nFixedRows; i++)
            p->y += GetRowHeight(i);
        for (i = idTopLeft.row; i < nRow; i++)
            p->y += GetRowHeight(i);
    }

    return TRUE;
}

BOOL CGridCtrl::GetCellOrigin(const CCellID& cell, LPPOINT p)
{
    return GetCellOrigin(cell.row, cell.col, p);
}

// Returns the bounding box of the cell
BOOL CGridCtrl::GetCellRect(const CCellID& cell, LPRECT pRect)
{
    return GetCellRect(cell.row, cell.col, pRect);
}

BOOL CGridCtrl::GetCellRect(int nRow, int nCol, LPRECT pRect)
{    
    CPoint CellOrigin;
    if (!GetCellOrigin(nRow, nCol, &CellOrigin)) return FALSE;

    pRect->left   = CellOrigin.x;
    pRect->top    = CellOrigin.y;
    pRect->right  = CellOrigin.x + GetColumnWidth(nCol)-1;
    pRect->bottom = CellOrigin.y + GetRowHeight(nRow)-1;

    //TRACE("Row %d, col %d: L %d, T %d, W %d, H %d:  %d,%d - %d,%d\n",
    //      nRow,nCol, CellOrigin.x, CellOrigin.y, GetColumnWidth(nCol), GetRowHeight(nRow),
    //      pRect->left, pRect->top, pRect->right, pRect->bottom);

    return TRUE;
}

// Returns the bounding box of a range of cells
BOOL CGridCtrl::GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect)
{
    CPoint MinOrigin,MaxOrigin;

    if (!GetCellOrigin(cellRange.GetMinRow(), cellRange.GetMinCol(), &MinOrigin)) 
        return FALSE;
    if (!GetCellOrigin(cellRange.GetMaxRow(), cellRange.GetMaxCol(), &MaxOrigin)) 
        return FALSE;

    lpRect->left   = MinOrigin.x;
    lpRect->top    = MinOrigin.y;
    lpRect->right  = MaxOrigin.x + GetColumnWidth(cellRange.GetMaxCol()-1);
    lpRect->bottom = MaxOrigin.y + GetRowHeight(cellRange.GetMaxRow()-1);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
// Grid attribute functions

LRESULT CGridCtrl::OnSetFont(WPARAM hFont, LPARAM /*lParam */)
{
    LRESULT result = Default();

    CFont *pFont = CFont::FromHandle((HFONT)hFont);
    if (pFont) {
        LOGFONT lf;
        pFont->GetLogFont(&lf);

        m_Font.DeleteObject();
        m_Font.CreateFontIndirect(&lf);
    }

    // Get the font size and hence the default cell size
    CDC* pDC = GetDC();
    if (pDC) {
        CFont* pOldFont = pDC->SelectObject(&m_Font);
        TEXTMETRIC tm;
        pDC->GetTextMetrics(&tm);
        m_nMargin = pDC->GetTextExtent(_T(" "),1).cx;
        pDC->SelectObject(pOldFont);
        ReleaseDC(pDC);

        m_nDefCellHeight = tm.tmHeight+tm.tmExternalLeading + 2*m_nMargin;
        m_nDefCellWidth  = tm.tmAveCharWidth*12 + 2*m_nMargin;
    }

    if (::IsWindow(GetSafeHwnd())) Invalidate();

    return result;
}

LRESULT CGridCtrl::OnGetFont(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return (LRESULT) m_Font.m_hObject;
}

BOOL CGridCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if (nHitTest == HTCLIENT)
    {
        switch (m_MouseMode) 
        {
            case MOUSE_OVER_COL_DIVIDE: SetCursor(::LoadCursor(NULL, IDC_SIZEWE)); break;
            case MOUSE_OVER_ROW_DIVIDE: SetCursor(::LoadCursor(NULL, IDC_SIZENS)); break;
            case MOUSE_DRAGGING:        break;
            default:                    SetCursor(::LoadCursor(NULL, IDC_ARROW));
        }
        return TRUE;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column count functions

BOOL CGridCtrl::SetFixedRowCount(int nFixedRows)
{
    ASSERT(nFixedRows >= 0);

    if (nFixedRows > GetRowCount()) 
        if (!SetRowCount(nFixedRows)) return FALSE;

    if (m_idCurrentCell.row < nFixedRows)
        SetFocusCell(-1,-1);

    m_nFixedRows = nFixedRows;

    if (GetSafeHwnd() && m_bAllowDraw)
        Invalidate();

    return TRUE;
}

BOOL CGridCtrl::SetFixedColumnCount(int nFixedCols)
{
    ASSERT(nFixedCols >= 0);
    if (nFixedCols > GetColumnCount())
        if (!SetColumnCount(nFixedCols)) return FALSE;

    if (m_idCurrentCell.col < nFixedCols)
        SetFocusCell(-1,-1);

    m_nFixedCols = nFixedCols;

    if (GetSafeHwnd() && m_bAllowDraw)
        Invalidate();

    return TRUE;
}

BOOL CGridCtrl::SetRowCount(int nRows)
{
    ASSERT(nRows >= 0);
    if (nRows == GetRowCount()) return TRUE;

    if (nRows < m_nFixedRows) 
        m_nFixedRows = nRows;

    if (m_idCurrentCell.row >= nRows)
        SetFocusCell(-1,-1);

    int addedRows = nRows - GetRowCount();

    // If we are about to lose rows, then we need to delete the GridCell objects 
    // in each column within each row
    if (addedRows < 0) {
        for (int row = nRows; row < m_nRows; row++)
        {
            // Delete cells
            for (int col = 0; col < m_nCols; col++) 
            {
                CGridCell* pCell = GetCell(row, col);
                if (pCell) {
                    EmptyCell(pCell, row, col);
                    delete pCell;
                }
            }
            // Delete rows
            GRID_ROW* pRow = m_RowData[row];
            if (pRow) delete pRow;
        }
    }

    // Change the number of rows.
    m_nRows = nRows;
    m_RowData.SetSize(m_nRows);
    m_arRowHeights.SetSize(nRows);

    // If we have just added rows, we need to construct new elements for each cell
    // and set the default row height
    if (addedRows > 0) {
        // initialize row heights and data
        int startRow = nRows - addedRows;
        for (int row = startRow; row < GetRowCount(); row++) {
            m_arRowHeights[row] = m_nDefCellHeight;
            m_RowData[row] = new GRID_ROW;
            m_RowData[row]->SetSize(m_nCols);
            for (int col = 0; col < m_nCols; col++)
            {
                GRID_ROW* pRow = m_RowData[row];
                if (pRow) pRow->SetAt(col, new CGridCell);
            }
        }
    }
    //else
    //    ResetSelectedRange();
    
    if (GetSafeHwnd() && m_bAllowDraw)
    {
        ResetScrollBars();
        Invalidate();
    }
    return TRUE;
}

BOOL CGridCtrl::SetColumnCount(int nCols)
{
    ASSERT(nCols >= 0);

    if (nCols == GetColumnCount()) return TRUE;

    if (nCols < m_nFixedCols) 
        m_nFixedCols = nCols;

    if (m_idCurrentCell.col >= nCols)
        SetFocusCell(-1,-1);

    int addedCols = nCols - GetColumnCount();
    m_nCols = nCols;
    m_arColWidths.SetSize(nCols);

    // If we are about to lose columns, then we need to delete the GridCell objects 
    // within each column
    if (addedCols < 0) {
        for (int row = 0; row < m_nRows; row++)
            for (int col = nCols; col < GetColumnCount(); col++)
            {
                CGridCell* pCell = GetCell(row, col);
                if (pCell) {
                    EmptyCell(pCell, row, col);
                    delete pCell;
                }
            }
    }

    // Change the number of columns in each row.
    for (int i = 0; i < m_nRows; i++)
        if (m_RowData[i]) m_RowData[i]->SetSize(nCols);

    // If we have just added columns, we need to construct new elements for each cell
    // and set the default column width
    if (addedCols > 0)
    {
        // initialized column widths
        int startCol = nCols - addedCols;
        for (int col = startCol; col < GetColumnCount(); col++)
            m_arColWidths[col] = m_nDefCellWidth;

        // initialise column data
        for (int row = 0; row < m_nRows; row++)
            for (int col = startCol; col < GetColumnCount(); col++)
            {
                GRID_ROW* pRow = m_RowData[row];
                if (pRow) pRow->SetAt(col, new CGridCell);
            }
    }
    //else    // check for selected cell ranges
    //    ResetSelectedRange();
    
    if (GetSafeHwnd() && m_bAllowDraw)
    {
        ResetScrollBars();
        Invalidate();
    }
    return TRUE;
}

// Insert a column at a given position, or add to end of columns (if nColumn = -1)
int CGridCtrl::InsertColumn(LPCTSTR strHeading, UINT nFormat, int nColumn /* = -1 */)
{
    // If the insertion is for a specific column, check it's within range.
    if (nColumn >= 0 && nColumn >= GetColumnCount()) return -1;

    // Gotta be able to at least _see_ some of the column.
    if (m_nRows < 1) SetRowCount(1);    

    if (nColumn < 0) {
        m_arColWidths.Add(0);
        for (int row = 0; row < m_nRows; row++) {
            GRID_ROW* pRow = m_RowData[row];
            if (!pRow) return -1;
            pRow->Add(new CGridCell);
        }
        nColumn = m_nCols;
    } else {
        m_arColWidths.InsertAt(nColumn, (UINT)0);
        for (int row = 0; row < m_nRows; row++) {
            GRID_ROW* pRow = m_RowData[row];
            if (!pRow) return -1;
            pRow->InsertAt(nColumn, new CGridCell);
        }
    }

    m_nCols++;

    // Initialise column data
    SetItemText(0, nColumn, strHeading);
    for (int row = 0; row < m_nRows; row++) {
        SetItemFormat(row, nColumn, nFormat);
    }

    // initialized column width
    m_arColWidths[nColumn] = GetTextExtent(strHeading).cx;
    ResetScrollBars();

    return nColumn;    // TODO: Add Memory exception handing
}

// Insert a row at a given position, or add to end of rows (if nRow = -1)
int CGridCtrl::InsertRow(LPCTSTR strHeading, int nRow /* = -1 */)
{
    // If the insertion is for a specific row, check it's within range.
    if (nRow >= 0 && nRow >= GetRowCount()) return -1;

    // Gotta be able to at least _see_ some of the row.
    if (m_nCols < 1) SetColumnCount(1);    

    // Adding a row to the bottom
    if (nRow < 0) {
        nRow = m_nRows;
        m_arRowHeights.Add(0);
        m_RowData.Add(new GRID_ROW);
    } else {
        m_arRowHeights.InsertAt(nRow, (UINT)0);
        m_RowData.InsertAt(nRow, new GRID_ROW);
    }
    
    m_nRows++;
    m_RowData[nRow]->SetSize(m_nCols);

    // Initialise cell data
    for (int col = 0; col < m_nCols; col++) {
        GRID_ROW* pRow = m_RowData[nRow];
        if (!pRow) return -1;
        CGridCell* pCell = new CGridCell;
        pRow->SetAt(col, pCell);
        if (pCell)
            pCell->nFormat = GetItemFormat(0, col);
    }

    // Set row title
    SetItemText(nRow, 0, strHeading);

    // initialized row height
    m_arRowHeights[nRow] = GetTextExtent(strHeading).cy;
    ResetScrollBars();

    return nRow;    // TODO: Add Memory exception handing
}

// Performs any cell cleanup necessary to maintain grid integrity
void CGridCtrl::EmptyCell(CGridCell* pCell, int nRow, int nCol)
{
    // Set the cells state to 0. If the cell is selected, this
    // will remove the cell from the selected list.
    SetItemState(nRow, nCol, 0);

    // Empty strings
    pCell->szText.Empty();
}

BOOL CGridCtrl::DeleteColumn(int nColumn)
{
    if (nColumn < 0 || nColumn >= GetColumnCount()) return FALSE;

    for (int row = 0; row < GetRowCount(); row++) {
        GRID_ROW* pRow = m_RowData[row];
        if (!pRow) return FALSE;

        CGridCell* pCell = pRow->GetAt(nColumn);
        if (pCell) {
            EmptyCell(pCell, row, nColumn);
            delete pCell;
        }
        pRow->RemoveAt(nColumn);
    }
    m_nCols--;
    if (nColumn < m_nFixedCols) m_nFixedCols--;

    if (nColumn == m_idCurrentCell.col)
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    else if (nColumn < m_idCurrentCell.col)
        m_idCurrentCell.col--;

    ResetScrollBars();
    return FALSE;
}

BOOL CGridCtrl::DeleteRow(int nRow)
{
    if (nRow < 0 || nRow >= GetRowCount()) return FALSE;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return FALSE;

    for (int col = 0; col < GetColumnCount(); col++) 
    {
        CGridCell* pCell = pRow->GetAt(col);
        if (pCell) {
            EmptyCell(pCell, nRow, col);
            delete pCell;
        }
    }
    delete pRow;
    m_RowData.RemoveAt(nRow);

    m_nRows--;
    if (nRow < m_nFixedRows) m_nFixedRows--;

    ResetScrollBars();

    if (nRow == m_idCurrentCell.row)
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    else if (nRow < m_idCurrentCell.row)
        m_idCurrentCell.row--;

    return TRUE;
}

// Removes all rows, columns and data from the grid.
void CGridCtrl::DeleteContents()
{
    m_arColWidths.RemoveAll();
    m_arRowHeights.RemoveAll();

    // Delete all cells in the grid
    for (int row = 0; row < m_nRows; row++) 
    {
        GRID_ROW* pRow = m_RowData[row];
        if (!pRow) continue;
        for (int col = 0; col < m_nCols; col++)
        {
            CGridCell* pCell = pRow->GetAt(col);
            if (pCell) {
                EmptyCell(pCell, row, col);  // TODO - this is a bit of a performance hit.
                delete pCell;                // better to call m_SelectedCells.RemoveAll()
            }                                // instead. This is safer for changes though.
        }
        delete pRow;
    }

    // Remove all rows
    m_RowData.RemoveAll();

    m_idCurrentCell.row = m_idCurrentCell.col = -1;
    m_nRows = m_nFixedRows = m_nCols = m_nFixedCols = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl data functions



/////////////////////////////////////////////////////////////////////////////
// CGridCtrl data functions

BOOL CGridCtrl::SetItem(const GV_ITEM* pItem)
{
    if (!pItem) return FALSE;
    CGridCell* pCell = GetCell(pItem->row, pItem->col);
    if (!pCell) return FALSE;

    if (pItem->mask & GVIF_TEXT)   pCell->szText  = pItem->szText;
    if (pItem->mask & GVIF_PARAM)  pCell->lParam  = pItem->lParam;
    if (pItem->mask & GVIF_STATE)  pCell->state   = pItem->state;
    if (pItem->mask & GVIF_FORMAT) pCell->nFormat = pItem->nFormat;

    return TRUE;
}

BOOL CGridCtrl::GetItem(GV_ITEM* pItem)
{
    if (!pItem) return FALSE;
    CGridCell* pCell = GetCell(pItem->row, pItem->col);
    if (!pCell) return FALSE;

    if (pItem->mask & GVIF_TEXT)   pItem->szText  = GetItemText(pItem->row, pItem->col);
    if (pItem->mask & GVIF_PARAM)  pItem->lParam  = pCell->lParam;
    if (pItem->mask & GVIF_STATE)  pItem->state   = pCell->state;
    if (pItem->mask & GVIF_FORMAT) pItem->nFormat = pCell->nFormat;

    return TRUE;
}

BOOL CGridCtrl::SetItemText(int nRow, int nCol, LPCTSTR str)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    if (!pCell) return FALSE;

    pCell->szText = str;
    return TRUE;
}

BOOL CGridCtrl::SetItemData(int nRow, int nCol, LPARAM lParam)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    if (!pCell) return FALSE;

    pCell->lParam = lParam;
    return TRUE;
}

LPARAM CGridCtrl::GetItemData(int nRow, int nCol) const
{    
    CGridCell* pCell = GetCell(nRow, nCol);
    if (!pCell) return (LPARAM) 0;

    return pCell->lParam;
}



BOOL CGridCtrl::SetItemState(int nRow, int nCol, UINT state)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) return FALSE;

    // If the cell is being unselected, remove it from the selected list
    if ((pCell->state & GVIS_SELECTED) && !(state & GVIS_SELECTED))
    {
        CCellID cell(nRow,nCol);
        for (int i = 0; i < m_SelectedCells.GetSize(); i++)
            if (m_SelectedCells[i] == cell) {
                m_SelectedCells.RemoveAt(i);
                break;
            }
   }
    // If cell is being selected, add it to the list of selected cells
    else if (!(pCell->state & GVIS_SELECTED) && (state & GVIS_SELECTED))
    {
        CCellID cell(nRow, nCol);
        m_SelectedCells.Add(cell);
    }

    // Set the cell's state
    pCell->state = state;

    return TRUE;
}

UINT CGridCtrl::GetItemState(int nRow, int nCol) const
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) return 0;

    return pCell->state;
}

BOOL CGridCtrl::SetItemFormat(int nRow, int nCol, UINT nFormat)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) return FALSE;

    pCell->nFormat = nFormat;
    return TRUE;
}

UINT CGridCtrl::GetItemFormat(int nRow, int nCol) const
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) return 0;

    return pCell->nFormat;
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column size functions

long CGridCtrl::GetVirtualWidth() const
{
    long lVirtualWidth = 0;
    int iColCount = GetColumnCount();
    for (int i = 0; i < iColCount; i++)
        lVirtualWidth += m_arColWidths[i];

    return lVirtualWidth;
}

long CGridCtrl::GetVirtualHeight() const
{
    long lVirtualHeight = 0;
    int iRowCount = GetRowCount();
    for (int i = 0; i < iRowCount; i++)
        lVirtualHeight += m_arRowHeights[i];

    return lVirtualHeight;
}

int CGridCtrl::GetRowHeight(int nRow) const
{
    ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows) return -1;

    return m_arRowHeights[nRow];
}

int CGridCtrl::GetColumnWidth(int nCol) const
{
    ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols) return -1;

    return m_arColWidths[nCol];
}

BOOL CGridCtrl::SetRowHeight(int nRow, int height)
{
    ASSERT(nRow >= 0 && nRow < m_nRows && height >= 0);
    if (nRow < 0 || nRow >= m_nRows || height < 0) return FALSE;

    m_arRowHeights[nRow] = height;    
    return TRUE;
}

BOOL CGridCtrl::SetColumnWidth(int nCol, int width)
{
    ASSERT(nCol >= 0 && nCol < m_nCols && width >= 0);
    if (nCol < 0 || nCol >= m_nCols || width < 0) return FALSE;

    m_arColWidths[nCol] = width;
    return TRUE;
}

int CGridCtrl::GetFixedRowHeight() const
{
    int nHeight = 0;
    for (int i = 0; i < m_nFixedRows; i++)
        nHeight += GetRowHeight(i);

    return nHeight;
}

int CGridCtrl::GetFixedColumnWidth() const
{
    int nWidth = 0;
    for (int i = 0; i < m_nFixedCols; i++)
        nWidth += GetColumnWidth(i);

    return nWidth;
}

BOOL CGridCtrl::AutoSizeColumn(int nCol)
{
    ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols) return FALSE;

    CSize size;
    CDC* pDC = GetDC();
    if (!pDC) return FALSE;

    int nWidth = 0;
    int nNumRows = GetRowCount();
    for (int nRow = 0; nRow < nNumRows; nRow++)
    {
        size = GetCellExtent(nRow, nCol, pDC);
        if (size.cx > nWidth) nWidth = size.cx;
    }

    m_arColWidths[nCol] = nWidth;

    ReleaseDC(pDC);
    ResetScrollBars();

    return TRUE;
}

BOOL CGridCtrl::AutoSizeRow(int nRow)
{
    ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows) return FALSE;

    CSize size;
    CDC* pDC = GetDC();
    if (!pDC) return FALSE;

    int nHeight = 0;
    int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {  
        size = GetCellExtent(nRow, nCol, pDC);
        if (size.cy > nHeight) nHeight = size.cy;
    }  
    m_arRowHeights[nRow] = nHeight;

    ReleaseDC(pDC);
    ResetScrollBars();

    return TRUE;
}

void CGridCtrl::AutoSizeColumns()
{
    int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < GetColumnCount(); nCol++)
        AutoSizeColumn(nCol);
}

void CGridCtrl::AutoSizeRows()
{
    int nNumRows = GetRowCount();
    for (int nRow = 0; nRow < nNumRows; nRow++)
        AutoSizeRow(nRow);
}

// sizes all rows and columns
// faster than calling both AutoSizeColumns() and AutoSizeRows()
void CGridCtrl::AutoSize()
{
    CDC* pDC = GetDC();
    if (!pDC) return;

    int nNumColumns = GetColumnCount();
    int nNumRows = GetRowCount();
    
    // initialize column widths to zero
    for (int nCol = 0; nCol < nNumColumns; nCol++)
        m_arColWidths[nCol] = 0;
    
    // initialize row heights to zero
    for (int nRow = 0; nRow < nNumRows; nRow++)
        m_arRowHeights[nRow] = 0;
    
    CSize size;
    for (int nCol = 0; nCol < nNumColumns; nCol++)
        for (int nRow = 0; nRow < nNumRows; nRow++)
        {
            size = GetCellExtent(nRow, nCol, pDC);
            if (size.cx > (int) m_arColWidths[nCol])  m_arColWidths[nCol] = size.cx;
            if (size.cy > (int) m_arRowHeights[nRow]) m_arRowHeights[nRow] = size.cy;
        }
    
    ReleaseDC(pDC);

    if (m_bAllowDraw) {
        ResetScrollBars();
        Invalidate();
    }
}

void CGridCtrl::ExpandToFit()
{
    CRect rect;
    GetClientRect(rect);

    long virtualWidth = GetVirtualWidth();
    if (virtualWidth < rect.Width())
    {
        int difference = rect.Width() - (int) virtualWidth -1  ; // -1 for padding
        int additionalColWidth = difference / GetColumnCount();
        for (int i = 0; i < GetColumnCount(); i++)
            m_arColWidths[i] += additionalColWidth;
        int leftOver = difference % GetColumnCount();
        for (int i = 0; i < leftOver; i++)
            m_arColWidths[i] += 1;
    }

    long virtualHeight = GetVirtualHeight();
    if (virtualHeight < rect.Height())
    {
        int difference = rect.Height() - (int) virtualHeight;
        int additionalRowHeight = difference / GetRowCount();
        for (int i = 0; i < GetRowCount(); i++)
            m_arRowHeights[i] += additionalRowHeight;
        int leftOver = difference % GetRowCount();
        for (int i = 0; i < leftOver; i++)
            m_arRowHeights[i] += 1;
    }

    if (m_bAllowDraw) Invalidate();
}

/////////////////////////////////////////////////////////////////////////////////////
// GridCtrl cell visibility tests and invalidation/redraw functions

BOOL CGridCtrl::IsCellVisible(CCellID cell)
{  
    return IsCellVisible(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellVisible(int nRow, int nCol)
{  
    int x,y;

    CCellID TopLeft;
    if (nCol >= GetFixedColumnCount() || nRow >= GetFixedRowCount())
    {
        TopLeft = GetTopleftNonFixedCell();
        if (nCol >= GetFixedColumnCount() && nCol < TopLeft.col) return FALSE;
        if (nRow >= GetFixedRowCount() && nRow < TopLeft.row) return FALSE;
    }

    CRect rect;
    GetClientRect(rect);
    if (nCol < GetFixedColumnCount())
    {
        x = 0;
        for (int i = 0; i <= nCol; i++) 
        {
            if (x >= rect.right) return FALSE;
            x += GetColumnWidth(i);    
        }
    } 
    else 
    {
        x = GetFixedColumnWidth();
        for (int i = TopLeft.col; i <= nCol; i++) 
        {
            if (x >= rect.right) return FALSE;
            x += GetColumnWidth(i);    
        }
    }

    if (nRow < GetFixedRowCount())
    {
        y = 0;
        for (int i = 0; i <= nRow; i++) 
        {
            if (y >= rect.bottom) return FALSE;
            y += GetRowHeight(i);    
        }
    } 
    else 
    {
        if (nRow < TopLeft.row) return FALSE;
        y = GetFixedRowHeight();
        for (int i = TopLeft.row; i <= nRow; i++) 
        {
            if (y >= rect.bottom) return FALSE;
            y += GetRowHeight(i);    
        }
    }

    return TRUE;
}

BOOL CGridCtrl::InvalidateCellRect(const CCellID& cell)
{
    ASSERT(IsValid(cell));
    if (!::IsWindow(GetSafeHwnd()) || !m_bAllowDraw) return FALSE;

    if (!IsCellVisible(cell.row, cell.col)) return FALSE;

    CRect rect;
    if (!GetCellRect(cell, rect)) return FALSE;
    rect.right++; rect.bottom++;
    InvalidateRect(rect, TRUE);

    return TRUE;
}

BOOL CGridCtrl::InvalidateCellRect(const CCellRange& cellRange)
{
    ASSERT(IsValid(cellRange));
    if (!::IsWindow(GetSafeHwnd()) || !m_bAllowDraw) return FALSE;

    CCellRange visibleCellRange = GetVisibleNonFixedCellRange().Intersect(cellRange);

    CRect rect;
    if (!GetCellRangeRect(visibleCellRange, rect)) return FALSE;

    rect.right++; rect.bottom++;
    InvalidateRect(rect, TRUE);

    return TRUE;
}


void CGridCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect rect;
    GetClientRect(rect);

    // If outside client area, return (unless we are drag n dropping)
    if (m_MouseMode != MOUSE_DRAGGING && !rect.PtInRect(point))
        return;

    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!(nFlags & MK_LBUTTON))
    {
        if (point.y < GetFixedRowHeight() && m_bAllowColumnResize)
        {
            CCellID idCurrentCell = GetCellFromPt(point);
            CPoint start;
            if (!GetCellOrigin(idCurrentCell, &start)) return;

            int endx = start.x + GetColumnWidth(idCurrentCell.col);

            if ((point.x - start.x <= SLOP && idCurrentCell.col != 0) || endx - point.x <= SLOP)
            {
                if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
                    SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
            else 
            {
                if (m_MouseMode != MOUSE_NOTHING)
                    SetCursor(::LoadCursor(NULL, IDC_ARROW));
                m_MouseMode = MOUSE_NOTHING;
            }
        }
        else if (point.x < GetFixedColumnWidth() && m_bAllowRowResize)
        {
            CCellID idCurrentCell = GetCellFromPt(point);
            CPoint start;
            if (!GetCellOrigin(idCurrentCell, &start)) return;

            int endy = start.y + GetRowHeight(idCurrentCell.row);

            if ((point.y - start.y <= SLOP && idCurrentCell.row != 0) || endy - point.y <= SLOP)
            {
                if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
                    SetCursor(::LoadCursor(NULL, IDC_SIZENS));
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
            else
            {
                if (m_MouseMode != MOUSE_NOTHING)
                    SetCursor(::LoadCursor(NULL, IDC_ARROW));
                m_MouseMode = MOUSE_NOTHING;
            }
        }
        else
        {
            if (m_MouseMode != MOUSE_NOTHING)
                SetCursor(::LoadCursor(NULL, IDC_ARROW));
            m_MouseMode = MOUSE_NOTHING;
        }

        m_LastMousePoint = point;
        return;
    }

    if (!IsValid(m_LeftClickDownCell))
    {
        m_LastMousePoint = point;
        return;
    }

    // If the left mouse button is down, the process appropriately
    if (nFlags & MK_LBUTTON) 
    {
        switch(m_MouseMode)
        {
            case MOUSE_SELECT_ALL:        break;

            case MOUSE_SELECT_COL:
            case MOUSE_SELECT_ROW:    
            case MOUSE_SELECT_CELLS:    {
                                            CCellID idCurrentCell = GetCellFromPt(point);
                                            if (!IsValid(idCurrentCell)) return;
                                            OnSelecting(idCurrentCell);
                                            if (idCurrentCell.row >= m_nFixedRows &&
                                                idCurrentCell.col >= m_nFixedCols)
                                                    SetFocusCell(idCurrentCell);
                                            break;
                                        }

            case MOUSE_SIZING_COL:        {
                                            CDC* pDC = GetDC();
                                            if (!pDC) break;

                                            CRect oldInvertedRect(m_LastMousePoint.x, rect.top, 
                                                                  m_LastMousePoint.x + 2, rect.bottom);
                                            pDC->InvertRect(&oldInvertedRect);
                                            CRect newInvertedRect(point.x, rect.top, 
                                                                  point.x + 2, rect.bottom);
                                            pDC->InvertRect(&newInvertedRect);
                                            ReleaseDC(pDC);
                                        }
                                        break;

            case MOUSE_SIZING_ROW:        {
                                            CDC* pDC = GetDC();
                                            if (!pDC) break;

                                            CRect oldInvertedRect(rect.left, m_LastMousePoint.y, 
                                                                  rect.right, m_LastMousePoint.y + 2);
                                            pDC->InvertRect(&oldInvertedRect);
                                            CRect newInvertedRect(rect.left, point.y, 
                                                                  rect.right, point.y + 2);
                                            pDC->InvertRect(&newInvertedRect);
                                            ReleaseDC(pDC);
                                        }
                                        break;

            

        }    
    }

    m_LastMousePoint = point;
}

void CGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) 
    {
        CCellID cell = GetCellFromPt(point);
        ASSERT(IsValid(cell));

        CPoint start;
        if (!GetCellOrigin(0, cell.col, &start)) return;

        if (point.x - start.x <= SLOP)     // Clicked right of border
            cell.col--;

        AutoSizeColumn(cell.col);
        Invalidate();
    } 
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE)
    {
        CCellID cell = GetCellFromPt(point);
        ASSERT(IsValid(cell));

        CPoint start;
        if (!GetCellOrigin(0, cell.col, &start)) return;

        if (point.y - start.y <= SLOP)     // Clicked below border
            cell.row--;

        AutoSizeRow(cell.row);
        Invalidate();
    }
    else if (m_MouseMode == MOUSE_NOTHING)
    {
        if (m_LeftClickDownCell.row >= m_nFixedRows && 
            IsValid(m_LeftClickDownCell) &&
            m_LeftClickDownCell.col >= m_nFixedCols)
        {
            OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, VK_LBUTTON);
        }
    }

    CWnd::OnLButtonDblClk(nFlags, point);
}

void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    HWND hOldFocusWnd = ::GetFocus();

    m_LeftClickDownPoint = point;
    m_LeftClickDownCell = GetCellFromPt(point);
    if (!IsValid(m_LeftClickDownCell)) return;

    m_SelectionStartCell = (nFlags & MK_SHIFT)? m_idCurrentCell : m_LeftClickDownCell;

    SetFocus();        // Auto-destroy any InPlaceEdit's

    // If the user clicks on the current cell, then prepare to edit it.
    // (If the user moves the mouse, then dragging occurs)
    if (m_LeftClickDownCell == m_idCurrentCell)
    {
        m_MouseMode = MOUSE_PREPARE_EDIT;
        return;
    } else {
        SetFocusCell(-1,-1);
        SetFocusCell(max(m_LeftClickDownCell.row, m_nFixedRows),
                     max(m_LeftClickDownCell.col, m_nFixedCols));
    }

    

    SetCapture();

    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) // sizing column
    {
        m_MouseMode = MOUSE_SIZING_COL;
        CPoint start;
        if (!GetCellOrigin(0, m_LeftClickDownCell.col, &start)) return;

        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(point.x, rect.top, point.x + 2, rect.bottom);

        CDC* pDC = GetDC();
        if (pDC) {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }

        if (point.x - start.x <= SLOP)        // clicked right of border
            if (!GetCellOrigin(0, --m_LeftClickDownCell.col, &start)) return;

        rect.left = start.x;
        ClientToScreen(rect);
        ClipCursor(rect);
    }
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) // sizing row
    {
        m_MouseMode = MOUSE_SIZING_ROW;
        CPoint start;
        if (!GetCellOrigin(m_LeftClickDownCell, &start)) return;

        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(rect.left, point.y, rect.right, point.y + 2);

        CDC* pDC = GetDC();
        if (pDC) {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }

        if (point.y - start.y <= SLOP)            // clicked below border
            if (!GetCellOrigin(--m_LeftClickDownCell.row, 0, &start)) return;

        rect.top = start.y;
        ClientToScreen(rect);
        ClipCursor(rect);
    }
    else // not sizing or editing -- selecting
    {    
        // If Ctrl pressed, save the current cell selection. This will get added
        // to the new cell selection at the end of the cell selection process
        m_PrevSelectedCells.RemoveAll();
        if (nFlags & MK_CONTROL)
            m_PrevSelectedCells.Copy(m_SelectedCells);
        
        if (m_LeftClickDownCell.row < GetFixedRowCount())
            OnFixedRowClick(m_LeftClickDownCell);
        else if (m_LeftClickDownCell.col < GetFixedColumnCount())
            OnFixedColumnClick(m_LeftClickDownCell);
        else
        {
            m_MouseMode = MOUSE_SELECT_CELLS;
            OnSelecting(m_LeftClickDownCell);
        }

        m_nTimerID = SetTimer(WM_LBUTTONDOWN, m_nTimerInterval, 0);
    }   
    m_LastMousePoint = point;
}

void CGridCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    CWnd::OnLButtonUp(nFlags, point);
    ClipCursor(NULL);

    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
    {
        ReleaseCapture();
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }

    // m_MouseMode == MOUSE_PREPARE_EDIT only if user clicked down on current cell
    // and then didn't move mouse before clicking up (releasing button)
    if (m_MouseMode == MOUSE_PREPARE_EDIT)    
    {
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, VK_LBUTTON);
    }
    // m_MouseMode == MOUSE_PREPARE_DRAG only if user clicked down on a selected cell
    // and then didn't move mouse before clicking up (releasing button)
    else if (m_MouseMode == MOUSE_PREPARE_DRAG) 
    {
        ResetSelectedRange();
    }
    else if (m_MouseMode == MOUSE_SIZING_COL)
    {
        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(m_LastMousePoint.x, rect.top, m_LastMousePoint.x + 2, rect.bottom);

        CDC* pDC = GetDC();
        if (pDC) {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }

        if (m_LeftClickDownPoint != point) 
        {   
            CPoint start;
            if (!GetCellOrigin(m_LeftClickDownCell, &start)) return;
            SetColumnWidth(m_LeftClickDownCell.col, point.x - start.x);
            ResetScrollBars();
            Invalidate();
        }
    }
    else if (m_MouseMode == MOUSE_SIZING_ROW)
    {
        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(rect.left, m_LastMousePoint.y, rect.right, m_LastMousePoint.y + 2);
    
        CDC* pDC = GetDC();
        if (pDC) {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }
    
        if (m_LeftClickDownPoint != point) 
        {
            CPoint start;
            if (!GetCellOrigin(m_LeftClickDownCell, &start)) return;

            SetRowHeight(m_LeftClickDownCell.row, point.y - start.y);
            ResetScrollBars();
            Invalidate();
        }
    } 

    m_MouseMode = MOUSE_NOTHING;
    SetCursor(::LoadCursor(NULL, IDC_ARROW));

    if (!IsValid(m_LeftClickDownCell)) return;

    GetParent()->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED), 
                                         (LPARAM) GetSafeHwnd());
}




void CGridCtrl::OnFixedRowClick(CCellID& cell)
{
    if (!IsValid(cell)) return;

    if (cell.col < GetFixedColumnCount()) 
    {
        m_MouseMode = MOUSE_SELECT_ALL;
        OnSelecting(cell);
    } 
    else 
    {
        m_MouseMode = MOUSE_SELECT_COL;
        OnSelecting(cell);
    }
}

void CGridCtrl::OnFixedColumnClick(CCellID& cell)
{
    if (!IsValid(cell)) return;


    if (cell.row < GetFixedRowCount()) {
        m_MouseMode = MOUSE_SELECT_ALL;
        OnSelecting(cell);
    } else {
        m_MouseMode = MOUSE_SELECT_ROW;
        OnSelecting(cell);
    }
}

// Gets the extent of the text pointed to by str (no CDC needed)
CSize CGridCtrl::GetTextExtent(LPCTSTR str)
{
    CDC* pDC = GetDC();
    if (!pDC) return CSize(0,0);

    CFont* pOldFont = pDC->SelectObject(&m_Font);
    CSize size = pDC->GetTextExtent(str);
    pDC->SelectObject(pOldFont);
    ReleaseDC(pDC);

    return size + CSize(2*m_nMargin, 2*m_nMargin);
}

CSize CGridCtrl::GetCellExtent(int nRow, int nCol, CDC* pDC)
{
    CFont* pOldFont = pDC->SelectObject(&m_Font);
    CSize size = pDC->GetTextExtent(GetItemText(nRow, nCol));
    pDC->SelectObject(pOldFont);

    size += CSize(4*m_nMargin, 2*m_nMargin);

    
    return CSize(size.cx , size.cy);
}

BOOL CGridCtrl::DrawFixedCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk)
{
    if (!m_bAllowDraw) return FALSE;

    GV_ITEM Item;
    Item.mask = GVIF_TEXT | GVIF_FORMAT ;
    Item.row = nRow;
    Item.col = nCol;
    if (!GetItem(&Item)) return FALSE;

    if (!pDC->IsPrinting() && bEraseBk)
        pDC->FillSolidRect(rect, GetFixedBkColor());
    pDC->SetTextColor(GetFixedTextColor());

    CFont Font, *pOldFont;
    int nSavedDC = pDC->SaveDC();

    CCellID FocusCell = GetFocusCell();

    if (IsValid(FocusCell) &&  (FocusCell.row == nRow || FocusCell.col == nCol))
    {
        rect.right++; rect.bottom++;
        pDC->DrawEdge(rect, EDGE_RAISED, BF_RECT);
        rect.DeflateRect(1,1);

        LOGFONT lf;
        m_Font.GetLogFont(&lf);
        lf.lfWeight = FW_BOLD;
        Font.CreateFontIndirect(&lf);
        pOldFont = pDC->SelectObject(&Font);
    }
    else
    {
        CPen lightpen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHIGHLIGHT)),
              darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
             *pOldPen = pDC->GetCurrentPen();
    
        pDC->SelectObject(&lightpen);
        pDC->MoveTo(rect.right, rect.top);
        pDC->LineTo(rect.left, rect.top);
        pDC->LineTo(rect.left, rect.bottom);

        pDC->SelectObject(&darkpen);
        pDC->MoveTo(rect.right, rect.top);
        pDC->LineTo(rect.right, rect.bottom);
        pDC->LineTo(rect.left, rect.bottom);

        pDC->SelectObject(pOldPen);
        rect.DeflateRect(1,1);

        pOldFont = pDC->SelectObject(&m_Font);
    }

    pDC->SetBkMode(TRANSPARENT);
    rect.DeflateRect(m_nMargin, 0);


    DrawText(pDC->m_hDC, Item.szText, -1, rect, Item.nFormat);

    pDC->RestoreDC(nSavedDC);
    return TRUE;
}

BOOL CGridCtrl::DrawCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk)
{
    if (!m_bAllowDraw) return FALSE;

    GV_ITEM Item;
    Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE ;
    Item.row = nRow;
    Item.col = nCol;
    if (!GetItem(&Item)) return FALSE;

    int nSavedDC = pDC->SaveDC();

    pDC->SetBkMode(TRANSPARENT);

    if (Item.state & GVIS_FOCUSED && !pDC->IsPrinting()) {
        rect.right++; rect.bottom++;    // FillSolidRect doesn't draw RHS or bottom
        if (bEraseBk) pDC->FillSolidRect(rect, GetTextBkColor());
        rect.right--; rect.bottom--;    
        pDC->SetTextColor(GetTextColor());

        pDC->SelectStockObject(BLACK_PEN);
        pDC->MoveTo(rect.left, rect.top);    
        pDC->LineTo(rect.right-1, rect.top);
        pDC->LineTo(rect.right-1, rect.bottom-1); 
        pDC->LineTo(rect.left, rect.bottom-1);
        pDC->LineTo(rect.left, rect.top);
        rect.DeflateRect(1,1);

    } else if (Item.state & GVIS_SELECTED && !pDC->IsPrinting()) {
        rect.right++; rect.bottom++;    // FillSolidRect doesn't draw RHS or bottom
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
        rect.right--; rect.bottom--;
        pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    } else {
        rect.right++; rect.bottom++;    // FillSolidRect doesn't draw RHS or bottom
        if (bEraseBk) pDC->FillSolidRect(rect, GetTextBkColor());
        rect.right--; rect.bottom--;
        pDC->SetTextColor(GetTextColor());
    }

    pDC->SelectObject(&m_Font);
    rect.DeflateRect(m_nMargin, 0);


    DrawText(pDC->m_hDC, Item.szText, -1, rect, Item.nFormat);

    pDC->RestoreDC(nSavedDC);
    return TRUE;
}

void CGridCtrl::OnEditCell(int nRow, int nCol, UINT nChar)
{
    CCellID cell(nRow, nCol);
    if (!m_bEditable ||!IsCellVisible(nRow, nCol) || !IsValid(cell)) 
        return;

    CRect rect;
    if (!GetCellRect(cell, rect)) return;

    GV_ITEM Item;
    Item.mask = GVIF_TEXT | GVIF_FORMAT;
    Item.row = nRow;
    Item.col = nCol;
    if (!GetItem(&Item)) return;

    DWORD dwStyle = ES_LEFT;
    if (Item.nFormat & DT_RIGHT) dwStyle = ES_RIGHT;
    else if (Item.nFormat & DT_CENTER) dwStyle = ES_CENTER;

	CFormulaEditor* F = (CFormulaEditor*)GetParent();
	F->OnFormulaEditorValue();
}

void CGridCtrl::OnEndEditCell(int nRow, int nCol, CString str)
{
    SetItemText(nRow, nCol, str);
}

CString CGridCtrl::GetItemText(int nRow, int nCol)
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) return "";

    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) return "";

    return pCell->szText;
}