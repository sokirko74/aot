//File: ToolTip2.cpp
//Implementation file for class CToolTip2
//Copyright (C) 2000  Dhandapani Ammasai( dammasai@hotmail.com )
//Mail your comments, criticism, bugs to the above e-mail address.
#include "stdafx.h"
#include "tooltip2.h"

IMPLEMENT_DYNAMIC( CToolTip2, CWnd );

BEGIN_MESSAGE_MAP(CToolTip2, CWnd)
	//{{AFX_MSG_MAP(CToolTip2)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//Default Constructor
CToolTip2::CToolTip2()
{
	m_szText = "";
	m_pParentWnd = NULL;
	m_bShowStatus = FALSE;
}

//Destructor
CToolTip2::~CToolTip2()
{
}

//Create Tool Tip
BOOL CToolTip2::Create(CWnd* pParentWnd)
{
	ASSERT(this != NULL );
	ASSERT(pParentWnd != NULL);

	m_pParentWnd = pParentWnd;
	//Create font
	m_font.CreateFont(15, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");
	CRect rectInitialSize(0,0,0,0);//Initial Window size. Will be dynamically changed later.
	return CreateEx(WS_EX_TOOLWINDOW , NULL, NULL,WS_POPUP |  WS_CHILD | WS_CLIPSIBLINGS,
		rectInitialSize,pParentWnd, NULL, NULL);
}

//Set tooltip text
void CToolTip2::SetText(const CString& rsText)
{
	ASSERT(this != NULL );
	m_szText = rsText;
	m_szText.TrimRight();
}

//Show tooltip
//rCurrentPoint specifies current mouse position and it is in client coordinates of parent window(Not in screen coordinates).
BOOL CToolTip2::Show(const CPoint& rCurrentPoint)
{
	ASSERT(this != NULL );
	ASSERT(m_hWnd != NULL );

	//Is text empty or tool tip already displayed?
	if ( m_szText.IsEmpty() || m_bShowStatus)
		return FALSE;

	m_ptCurrent = rCurrentPoint;
	m_bShowStatus = TRUE;
	//Show tool tip
	DisplayToolTip(rCurrentPoint);
	return TRUE;
}

//Close the tooltip
void CToolTip2::Close()
{
	ASSERT(this != NULL );
	ASSERT(m_hWnd != NULL );

	ShowWindow(SW_HIDE); //Hide tooltip
	m_bShowStatus = FALSE;
}

void CToolTip2::OnPaint()
{
	CPaintDC dc(this);

	DisplayToolTip(m_ptCurrent);
}

//Display tooltip
void CToolTip2::DisplayToolTip(const CPoint& rCurrentPoint)
{
	CDC* pDC = GetDC();

	CBrush		*pOldBrush;

	CFont *pOldFont;
	pOldFont = pDC->SelectObject(&m_font);
	
	//CSize size = pDC->GetTextExtent(m_szText);
	//pDC->LPtoDP(&size);
	int x = 0;
	int nLargest = 0;
	CString sLine;
	CSize size;
	while(GetLine(m_szText, &sLine, x))
	{
		size = pDC->GetTextExtent(sLine);
		pDC->LPtoDP(&size);
		if(size.cx > nLargest)
			nLargest = size.cx;
		x++;
	}
	int nHeight = size.cy;
	size.cx = nLargest;
	size.cy *= x;

	
	
	//Form tooltip rectangle
	CRect rectToolTip(rCurrentPoint.x, rCurrentPoint.y, rCurrentPoint.x+size.cx+7, rCurrentPoint.y+size.cy+2);

	//Draw Tooltip Rect and Text
	//
	pDC->SetBkMode(TRANSPARENT);
	CBrush brushToolTip(GetSysColor(COLOR_INFOBK));
	pOldBrush = pDC->SelectObject(&brushToolTip);

	//Create and select thick black pen
	CPen penBlack(PS_SOLID, 0, COLORREF(RGB(0, 0, 0)));
	CPen* pOldPen = pDC->SelectObject(&penBlack);

	//Draw rectangle filled with COLOR_INFOBK
	pDC->Rectangle(0,0,rectToolTip.Width(),rectToolTip.Height());

	//Draw tooltip text
   	pDC->SetTextColor( GetSysColor(COLOR_INFOTEXT) );//Tool Tip color set in control panel settings
	pDC->SetTextAlign(TA_LEFT);
	//pDC->TextOut(3,1, m_szText);
	x = 0;
	while(GetLine(m_szText, &sLine, x))
	{
		pDC->TextOut(3,1 + (x * nHeight), sLine);
		x++;
	}


	CRect rectWnd = rectToolTip;
	m_pParentWnd->ClientToScreen(rectWnd); //Convert from client to screen coordinates
	CPoint ptToolTipLeft = rectWnd.TopLeft();

	//Now display tooltip
	SetWindowPos(&wndTop,ptToolTipLeft.x+1, ptToolTipLeft.y+1, rectWnd.Width(), 
rectWnd.Height(),SWP_SHOWWINDOW|SWP_NOOWNERZORDER|SWP_NOACTIVATE);

	//put back old objects
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);

	ReleaseDC(pDC);
}



BOOL CToolTip2::GetLine(CString sSource, CString *sDest, int nLineNumber)
{
	int x = 0;
	int nStart = 0;
	while(x < nLineNumber)
	{
		nStart = sSource.Find('\n', nStart) + 1; 

		if(nStart == 0)
		{
			(*sDest) = sSource;
			return FALSE;
		}
		x++;
	}


	// go from the offset, to the next \n or end of
	// string
	int nEnd = sSource.Find('\n', nStart);
	if(nEnd == -1)
		nEnd = sSource.GetLength();

	(*sDest) = sSource.Mid(nStart, nEnd - nStart);
	return TRUE;
}
