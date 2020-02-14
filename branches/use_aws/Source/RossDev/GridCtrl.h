/////////////////////////////////////////////////////////////////////////////
// MFC Grid Control header file
// Written by Chris Maunder (Chris.Maunder@cbr.clw.csiro.au)
// Copyright (c) 1998.
// Adopted by Alexey Sokirko

#if !defined(AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

// The code contained in this file is based on the original
// WorldCom Grid control written by Joe Willcoxson,
//		mailto:chinajoe@aol.com
//		http://users.aol.com/chinajoe

class CCellID
{    
// Attributes
public:
	int row, col;

// Operations
public:
	CCellID(int nRow = -1, int nCol = -1) : row(nRow), col(nCol) {}

	int IsValid() const { return (row >= 0 && col >= 0); }
	int operator==(const CCellID& rhs)    { return (row == rhs.row && col == rhs.col); }
	int operator!=(const CCellID& rhs)    { return !operator==(rhs); }
};

class CCellRange
{ 
public:
	
	CCellRange(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1)
	{
		Set(nMinRow, nMinCol, nMaxRow, nMaxCol);
	}

	void Set(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1);
	
	int  IsValid() const;
	int  InRange(int row, int col) const;
	int  InRange(const CCellID& cellID) const;
	
	CCellID  GetTopLeft() const;
	CCellRange  Intersect(const CCellRange& rhs) const;
	
	int GetMinRow() const {return m_nMinRow;}
	void SetMinRow(int minRow) {m_nMinRow = minRow;}
	
	int GetMinCol() const {return m_nMinCol;}
	void SetMinCol(int minCol) {m_nMinCol = minCol;}
	
	int GetMaxRow() const {return m_nMaxRow;}
	void SetMaxRow(int maxRow) {m_nMaxRow = maxRow;}
	
	int GetMaxCol() const {return m_nMaxCol;}
	void SetMaxCol(int maxCol) {m_nMaxCol = maxCol;}

	int GetRowSpan() const {return m_nMaxRow - m_nMinRow + 1;}
	int GetColSpan() const {return m_nMaxCol - m_nMinCol + 1;}
	
	int  operator==(const CCellRange& rhs);
	int  operator!=(const CCellRange& rhs);
	
protected:
	int m_nMinRow;
	int m_nMinCol;
	int m_nMaxRow;
	int m_nMaxCol;
};

inline void CCellRange::Set(int minRow, int minCol, int maxRow, int maxCol)
{
 	m_nMinRow = minRow;
 	m_nMinCol = minCol;
 	m_nMaxRow = maxRow;
 	m_nMaxCol = maxCol;
}

inline int CCellRange::operator==(const CCellRange& rhs)
{
 	return ((m_nMinRow == rhs.m_nMinRow) && (m_nMinCol == rhs.m_nMinCol) &&
 			(m_nMaxRow == rhs.m_nMaxRow) && (m_nMaxCol == rhs.m_nMaxCol));
}

inline int CCellRange::operator!=(const CCellRange& rhs)
{
 	return !operator==(rhs);
}

inline int CCellRange::IsValid() const
{
 	return (m_nMinRow >= 0 && m_nMinCol >= 0 && m_nMaxRow >= 0 && m_nMaxCol >= 0 &&
 			m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol);
}

inline int CCellRange::InRange(int row, int col) const
{
 	return (row >= m_nMinRow && row <= m_nMaxRow && col >= m_nMinCol && col <= m_nMaxCol);
}

inline int CCellRange::InRange(const CCellID& cellID) const
{
 	return InRange(cellID.row, cellID.col);
}

inline CCellID CCellRange::GetTopLeft() const
{
 	return CCellID(m_nMinRow, m_nMinCol);
}

inline CCellRange CCellRange::Intersect(const CCellRange& rhs) const
{
 	return CCellRange(max(m_nMinRow,rhs.m_nMinRow), max(m_nMinCol,rhs.m_nMinCol),
 					  min(m_nMaxRow,rhs.m_nMaxRow), min(m_nMaxCol,rhs.m_nMaxCol));
}



// Use this as the classname when inserting this control as a custom control
// in the MSVC++ dialog editor
#define GRIDCTRL_CLASSNAME _T("MFCGridCtrl")

#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

// Used for Get/SetItem calls.
typedef struct _GV_ITEM { 
    int     row,col;     // Row and Column of item
    UINT    mask;        // Mask for use in getting/setting cell data
    UINT    state;       // cell state (focus/hilighted etc)
    UINT    nFormat;     // Format of cell
    CString szText;      // Text in cell
    LPARAM  lParam;      // 32-bit value to associate with item 
} GV_ITEM; 

// Grid line selection
#define GVL_NONE                0
#define GVL_HORZ                1
#define GVL_VERT                2
#define GVL_BOTH                3

// Cell data mask
#define GVIF_TEXT               LVIF_TEXT
#define GVIF_PARAM              LVIF_PARAM
#define GVIF_STATE              LVIF_STATE
#define GVIF_FORMAT             (GVIF_STATE<<1)

// Cell states
#define GVIS_FOCUSED            LVIS_FOCUSED
#define GVIS_SELECTED           LVIS_SELECTED
#define GVIS_DROPHILITED        LVIS_DROPHILITED

// Cell Searching options
#define GVNI_FOCUSED            LVNI_FOCUSED
#define GVNI_SELECTED           LVNI_SELECTED
#define GVNI_DROPHILITED        LVNI_DROPHILITED

#define GVNI_ABOVE              LVNI_ABOVE
#define GVNI_BELOW              LVNI_BELOW
#define GVNI_TOLEFT             LVNI_TOLEFT
#define GVNI_TORIGHT            LVNI_TORIGHT

// Hit test values (not yet implemented)
#define GVHT_DATA               0x0000
#define GVHT_TOPLEFT            0x0001
#define GVHT_COLHDR             0x0002
#define GVHT_ROWHDR             0x0004
#define GVHT_COLSIZER           0x0008
#define GVHT_ROWSIZER           0x0010
#define GVHT_LEFT               0x0020
#define GVHT_RIGHT              0x0040
#define GVHT_ABOVE              0x0080
#define GVHT_BELOW              0x0100


// Each cell contains one of these. Fields "row" and "column" are not stored since we
// will usually have acces to them in other ways, and they are an extra 8 bytes per
// cell that is probably unnecessary.

class CGridCell : public CObject
{
public:
    CGridCell():state(0),nFormat(0),szText(),lParam(0) {};

    UINT    state;         // Cell state (selected/focus etc)
    UINT    nFormat;       // Cell format
    CString szText;        // Cell text (or binary data if you wish...)
    LPARAM  lParam;        // 32-bit value to associate with item
}; 


// storage typedef for each row in the grid
typedef CTypedPtrArray<CObArray, CGridCell*> GRID_ROW;

// DDX_GridControl is used where a DDX_Control call is needed. In some strange
// situations the usual DDX_Control does not result in CGridCtrl::SubclassWindow
// or CGridCtrl::PreSubclassWindow being called. Using this version calls 
// CGridCtrl::SubclassWindow directly - ensuring that cell metrics are set properly
class CGridCtrl;
void AFXAPI DDX_GridControl(CDataExchange* pDX, int nIDC, CGridCtrl& rControl);

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl window

class CGridCtrl : public CWnd
{
// Construction
public:
    CGridCtrl(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);
    DECLARE_DYNCREATE(CGridCtrl)

    BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);
    BOOL SubclassWindow(HWND hWnd);

// Attributes
public:
    int  GetRowCount() const                    { return m_nRows; }
    int  GetColumnCount() const                 { return m_nCols; }
    int  GetFixedRowCount() const               { return m_nFixedRows; }
    int  GetFixedColumnCount() const            { return m_nFixedCols; }
    BOOL SetRowCount(int nRows = 10);
    BOOL SetColumnCount(int nCols = 10);
    BOOL SetFixedRowCount(int nFixedRows = 1);
    BOOL SetFixedColumnCount(int nFixedCols = 1);

    int  GetRowHeight(int nRow) const;
    BOOL SetRowHeight(int row, int height);
    int  GetColumnWidth(int nCol) const;
    BOOL SetColumnWidth(int col, int width);

    BOOL GetCellOrigin(int nRow, int nCol, LPPOINT p);
    BOOL GetCellOrigin(const CCellID& cell, LPPOINT p);
    BOOL GetCellRect(int nRow, int nCol, LPRECT pRect);
    BOOL GetCellRect(const CCellID& cell, LPRECT pRect);

    int  GetFixedRowHeight() const;
    int  GetFixedColumnWidth() const;
    long GetVirtualWidth() const;
    long GetVirtualHeight() const;

    void   SetTextColor(COLORREF clr)             { m_crTextColour = clr;             }
    COLORREF GetTextColor()                       { return m_crTextColour;            }
    void   SetTextBkColor(COLORREF clr)           { m_crTextBkColour = clr;           }
    COLORREF GetTextBkColor()                     { return m_crTextBkColour;          }
    void   SetBkColor(COLORREF clr)               { m_crBkColour = clr;               }
    COLORREF GetBkColor()                         { return m_crBkColour;              }
    void   SetFixedTextColor(COLORREF clr)        { m_crFixedTextColour = clr;        }
    COLORREF GetFixedTextColor()                  { return m_crFixedTextColour;       }
    void SetFixedBkColor(COLORREF clr)            { m_crFixedBkColour = clr;          }
    COLORREF GetFixedBkColor()                    { return m_crFixedBkColour;         } 

#ifdef USE_SELECTION_MAPS
    int GetSelectedCount()                        { return m_SelectedCellMap.GetCount(); }
#else
    int GetSelectedCount()                        { return m_SelectedCells.GetSize(); }
#endif
    CCellID GetFocusCell()                        { return m_idCurrentCell;           }


    void SetGridLines(int nWhichLines = GVL_BOTH) { m_nGridLines = nWhichLines; 
                                                    if (::IsWindow(GetSafeHwnd())) Invalidate(); }
    int  GetGridLines()                           { return m_nGridLines;              }

    void SetEditable(BOOL bEditable = TRUE)       { m_bEditable = bEditable;          }
    BOOL IsEditable()                             { return m_bEditable;               }
    void SetModified(BOOL bModified = TRUE)       { m_bModified = bModified;          }
    BOOL GetModified()                            { return m_bModified;               }
    void EnableSelection(BOOL bEnable = TRUE)     { ResetSelectedRange(); m_bEnableSelection = bEnable; ResetSelectedRange(); }
    BOOL IsSelectable()                           { return m_bEnableSelection;        }
    void SetRowResize(BOOL bResize = TRUE)        { m_bAllowRowResize = bResize;      }
    BOOL GetRowResize()                           { return m_bAllowRowResize;         }
    void SetColumnResize(BOOL bResize = TRUE)     { m_bAllowColumnResize = bResize;   }
    BOOL GetColumnResize()                        { return m_bAllowColumnResize;      }

    BOOL SetItem(const GV_ITEM* pItem);
    BOOL GetItem(GV_ITEM* pItem);
    BOOL SetItemText(int nRow, int nCol, LPCTSTR str);
    // virtual CString GetItemText(int nRow, int nCol) - moved to "Overrides" section
    BOOL   SetItemData(int nRow, int nCol, LPARAM lParam);
    LPARAM GetItemData(int nRow, int nCol) const;
    BOOL   SetItemState(int nRow, int nCol, UINT state);
    UINT   GetItemState(int nRow, int nCol) const;
    BOOL   SetItemFormat(int nRow, int nCol, UINT nFormat);
    UINT   GetItemFormat(int nRow, int nCol) const;

// Operations
public:
    int  InsertColumn(LPCTSTR strHeading, UINT nFormat, int nColumn = -1);
    int  InsertRow(LPCTSTR strHeading, int nRow = -1);
    BOOL DeleteColumn(int nColumn);
    BOOL DeleteRow(int nRow);
    void DeleteContents();

    BOOL AutoSizeRow(int nRow);
    BOOL AutoSizeColumn(int nCol);
    void AutoSizeRows();
    void AutoSizeColumns();
    void AutoSize();
    void ExpandToFit();

    // SetRedraw stops/starts redraws on things like changing the # rows/columns
    // and autosizing, but not for user-intervention such as resizes
    void SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars = FALSE);
    BOOL RedrawCell(int nRow, int nCol, CDC* pDC = NULL);
    BOOL RedrawCell(const CCellID& cell, CDC* pDC = NULL);
    BOOL RedrawRow(int row);
    BOOL RedrawColumn(int col);


    CCellRange GetCellRange();
    void SetSelectedRange(const CCellRange& Range, BOOL bForceRepaint = FALSE);
    void SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
                          BOOL bForceRepaint = FALSE);
    BOOL IsValid(int nRow, int nCol);
    BOOL IsValid(const CCellID& cell);
    BOOL IsValid(const CCellRange& range);
	virtual CString GetItemText(int nRow, int nCol);


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrl)
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL
    public:

// Implementation
public:
    virtual ~CGridCtrl();

protected:
    BOOL RegisterWindowClass();

    BOOL IsCellVisible(int nRow, int nCol);
    BOOL IsCellVisible(CCellID cell);

    BOOL InvalidateCellRect(const CCellID& cell);
    BOOL InvalidateCellRect(const CCellRange& cellRange);
    void EraseBkgnd(CDC* pDC);

    CSize GetTextExtent(LPCTSTR str);
    BOOL GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect);

    CGridCell* GetCell(int nRow, int nCol) const;
    BOOL SetCell(int nRow, int nCol, CGridCell* pCell);

    int  SetMouseMode(int nMode) { int nOldMode = m_MouseMode; m_MouseMode = nMode; return nOldMode; }
    int  GetMouseMode()          { return m_MouseMode; }

    CCellID GetCellFromPt(CPoint point);
    CCellID GetTopleftNonFixedCell();
    CCellRange GetUnobstructedNonFixedCellRange();
    CCellRange GetVisibleNonFixedCellRange(LPRECT pRect = NULL);

    CCellID SetFocusCell(CCellID cell);
    CCellID SetFocusCell(int nRow, int nCol);

    void ResetSelectedRange();
    void ResetScrollBars();



// Overrrides
protected:

    
    // Mouse Clicks
    virtual void  OnFixedColumnClick(CCellID& cell);
    virtual void  OnFixedRowClick(CCellID& cell);

    // Editing
    
    virtual CSize GetCellExtent(int nRow, int nCol, CDC* pDC);
    virtual void  OnEndEditCell(int nRow, int nCol, CString str);
    virtual void  OnEditCell(int nRow, int nCol, UINT nChar);

    // Drawing
    virtual void  OnDraw(CDC& origDC);
    virtual BOOL  DrawFixedCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk=FALSE);
    virtual BOOL  DrawCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk=FALSE);

    // Cleanup
    virtual void EmptyCell(CGridCell* pCell, int nRow, int nCol);

// Attributes
protected:
    // General attributes
    COLORREF    m_crTextColour, m_crTextBkColour, m_crBkColour,   // Grid colours
                m_crFixedTextColour, m_crFixedBkColour;
    COLORREF    m_crWindowText, m_crWindowColour, m_cr3DFace,     // System colours
                m_crShadow;    

    int         m_nGridLines;
    BOOL        m_bEditable;
    BOOL        m_bModified;
    BOOL        m_bAllowDraw;
    BOOL        m_bEnableSelection;

    // Cell size details
    int         m_nRows, m_nFixedRows, m_nCols, m_nFixedCols;
    CUIntArray  m_arRowHeights, m_arColWidths;
    int         m_nMargin;
    int         m_nDefCellWidth, m_nDefCellHeight;
    int         m_nVScrollMax, m_nHScrollMax;

    // Fonts and images
    CFont       m_Font;

    // Cell data
    CTypedPtrArray<CObArray, GRID_ROW*> m_RowData;

    // Mouse operations such as cell selection
    int         m_MouseMode;
    CPoint      m_LeftClickDownPoint, m_LastMousePoint;
    CCellID     m_LeftClickDownCell, m_SelectionStartCell;
#ifdef USE_SELECTION_MAPS
    CMap<DWORD,DWORD, CCellID, CCellID&> m_SelectedCellMap, m_PrevSelectedCellMap;
#else
    CArray<CCellID, CCellID&> m_SelectedCells, m_PrevSelectedCells;
#endif
    CCellID     m_idCurrentCell;
    int         m_nTimerID;
    int         m_nTimerInterval;
    BOOL        m_bAllowRowResize, m_bAllowColumnResize;

    // Drag and drop
    CWnd*       m_pDropWnd;

    // Printing information
    CSize       m_CharSize;
    int         m_nPageHeight;
    CSize       m_LogicalPageSize,      // Page size in gridctrl units.
                m_PaperSize;            // Page size in device units.


protected:
    void SelectAllCells();
    void SelectColumns(CCellID currentCell);
    void SelectRows(CCellID currentCell);
    void SelectCells(CCellID currentCell);
    void OnSelecting(const CCellID& currentCell);

    // Generated message  functions
    //{{AFX_MSG(CGridCtrl)
    afx_msg void OnPaint();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    //}}AFX_MSG
    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
    afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

    enum eMouseModes { MOUSE_NOTHING, MOUSE_SELECT_ALL, MOUSE_SELECT_COL, MOUSE_SELECT_ROW,
                       MOUSE_SELECT_CELLS, MOUSE_SCROLLING_CELLS,
                       MOUSE_OVER_ROW_DIVIDE, MOUSE_SIZING_ROW, 
                       MOUSE_OVER_COL_DIVIDE, MOUSE_SIZING_COL,
                       MOUSE_PREPARE_EDIT, MOUSE_PREPARE_DRAG, MOUSE_DRAGGING};
};

inline CGridCell* CGridCtrl::GetCell(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) return NULL;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return NULL;
    return pRow->GetAt(nCol);
}

inline BOOL CGridCtrl::SetCell(int nRow, int nCol, CGridCell* pCell)
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) return FALSE;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return FALSE;

    pRow->SetAt(nCol, pCell);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
