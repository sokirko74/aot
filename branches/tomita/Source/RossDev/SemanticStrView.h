// graphtest3View.h : interface of the CSemanticStrView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHTEST4VIEW_H__6DD15255_FD3E_11D2_8719_B8D0AF764170__INCLUDED_)
#define AFX_GRAPHTEST4VIEW_H__6DD15255_FD3E_11D2_8719_B8D0AF764170__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stdafx.h"
#include <tk.h>
#include "SemanticStrDoc.h"


class  CRusSemStructure;

class CSemanticStrView : public CView
{

protected: // create from serialization only
	DECLARE_DYNCREATE(CSemanticStrView)

// Attributes
private: 
	CString					m_Title;
	CMenu					m_Menu;	
	// слоты для рисования графов
	
	Tk_Window            m_tkwin;
	HWND                 m_tkhwnd;

public:
	CString              m_tkname;
	CSemanticStrView();
	~CSemanticStrView();
	
	CSemanticStrDoc*	GetDoc();
	void				BuildTclGraph(CString Graph);
	void				ShowGraph();
	void				OpenAllRosses();
	void				PasteClipboard();
	CRossDoc*			FindRossDoc(DictTypeEnum RossType);
      

	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSemanticStrView)
	public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
private:
	void		OpenRossDocIfNeeded(DictTypeEnum RossType);
	CString		GetNormalizedRossPath(DictTypeEnum RossType) const;
	void	GetJavaGraph();
public:
	
	
	
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message  functions
protected:
	//{{AFX_MSG(CSemanticStrView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	
	//}}AFX_MSG

protected:
	afx_msg void OnBackBtn();
	afx_msg void OnForwardBtn();

	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHTEST3VIEW_H__6DD15255_FD3E_11D2_8719_B8D0AF764170__INCLUDED_)
