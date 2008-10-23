// graphtest3View.h : interface of the CHierarchyView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHTEST3VIEW_H__6DD15255_FD3E_11D2_8719_B8D0AF764170__INCLUDED_)
#define AFX_GRAPHTEST3VIEW_H__6DD15255_FD3E_11D2_8719_B8D0AF764170__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "HierarchyDoc.h"

#include "..\tk_source\tk8.0.5\generic\tk.h"

class CHierarchyView : public CView
{
public:




protected: // create from serialization only
	CHierarchyView();
	DECLARE_DYNCREATE(CHierarchyView)

// Attributes
public:
	
	CHierarchyDoc* GetDocument();
	size_t m_SourceNo;
	int    m_NewNodeCount;

	CString   m_tkname;
	Tk_Window m_tkwin;
	HWND      m_tkhwnd;


	//vector<CNode>     Nodes;
	//vector<CRelation> Relations;

	bool FindRelation (size_t  node1, size_t node2);
	bool FindOmniDirectionRelation (size_t  node1, size_t node2);
	void CreateNode(const char* Name, CPoint p);
	void CreateRelation(int node1, int node2, bool bWork);
    void ShowGraph();
	


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHierarchyView)
	public:
	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHierarchyView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message  functions
protected:
	//{{AFX_MSG(CHierarchyView)
	afx_msg void OnButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in graphtest3View.cpp
inline CHierarchyDoc* CHierarchyView::GetDocument()
   { return (CHierarchyDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHTEST3VIEW_H__6DD15255_FD3E_11D2_8719_B8D0AF764170__INCLUDED_)
