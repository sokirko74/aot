//File: CToolTip2.h
//Header file for class CToolTip2
//Copyright (C) 2000  Dhandapani Ammasai( dammasai@hotmail.com )
//Mail your comments, criticism, bugs to the above e-mail address.
#ifndef _TOOL_TIP_2_H_
#define _TOOL_TIP_2_H_
#include "stdafx.h"

class CToolTip2 : public CWnd
{
DECLARE_DYNAMIC( CToolTip2 );
private: //data members
	//Tooltip Text
	CString m_szText;
	//Parent Window
	CWnd* m_pParentWnd;
	//Show Status
	BOOL m_bShowStatus;
	//Current point where tooltip is displayed
	CPoint m_ptCurrent;
	//Font
	CFont m_font;
private: //Methods
	//Display Tooltip Window
	void DisplayToolTip(const CPoint& rCurrentPoint);
	//Copy constructor
	CToolTip2(const CToolTip2&);
	//Assignment operator
	CToolTip2& operator=(const CToolTip2&);
public: //Methods
	//Default Constructor
	CToolTip2();
	//Destructor
	~CToolTip2();
	//Create tootip
	BOOL Create(CWnd* pParentWnd);
	//Set tooltip text
	void SetText(const CString&);
	//Show the tooltip
	BOOL Show(const CPoint&);
	//Close the tooltip
	void Close();
protected:
	BOOL GetLine(CString sSource, CString * sDest, int nLineNumber);
	// Generated message map functions
	//{{AFX_MSG(CToolTip2)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
