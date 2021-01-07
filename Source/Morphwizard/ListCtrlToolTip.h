#pragma once

#include "tooltip2.h"

// CListCtrlToolTip

class CListCtrlToolTip : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlToolTip)
	CToolTipCtrl	m_tooltip;
public:
	std::vector<CStringW>		m_ToolTips;
	CListCtrlToolTip();
	virtual ~CListCtrlToolTip();
	void clear();

	
	

protected:
	CString (*m_fnTTTextRetriever)( int);
	CToolTip2	m_ToolTip;
	CString		m_strToolTipText;
	int			m_nPrevItem;			// item over which the mouse cursor was
	BOOL		m_bMouseOver;

	void SetToolTipText(LPCTSTR string, CPoint point);
	int ItemFromPoint(CPoint point);
	LRESULT OnMouseHover (WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void ShowToolTip(CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};


