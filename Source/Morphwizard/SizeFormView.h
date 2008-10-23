#if !defined(AFX_SIZEFORMVIEW_H__58122723_B5C9_11D2_A0A1_000000000000__INCLUDED_)
#define AFX_SIZEFORMVIEW_H__58122723_B5C9_11D2_A0A1_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SizeFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSizeFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "OXResizeGadget.h"

class CSizeFormView : public CFormView
{
protected:
	CSizeFormView( LPCTSTR lpszTemplateName );
	CSizeFormView( UINT nIDTemplate );
	DECLARE_DYNAMIC(CSizeFormView)

// Form Data
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSizeFormView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwRequestedStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSizeFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// Generated message map functions
	//{{AFX_MSG(CSizeFormView)
	afx_msg void OnSize(UINT nType, int cx, int cy); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	GADGETRESIZEHANDLE m_hGadgetResize;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIZEFORMVIEW_H__58122723_B5C9_11D2_A0A1_000000000000__INCLUDED_)
