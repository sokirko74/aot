#pragma once 
#include "HierarchyDoc.h"

#include "windows/tk_source/tk8.0.5/generic/tk.h"

class CHierarchyView : public CView
{
	CTclTkWrapper&		m_TclInterp;
	void SwitchMainTkWindow();
protected: // create from serialization only
	CHierarchyView();
	DECLARE_DYNCREATE(CHierarchyView)

// Attributes
public:
	
	CHierarchyDoc* GetDocument();
	size_t m_SourceNo;
	int    m_NewNodeCount;

	CTopTkWindow m_TopTKWindow;


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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in graphtest3View.cpp
inline CHierarchyDoc* CHierarchyView::GetDocument()
   { return (CHierarchyDoc*)m_pDocument; }
#endif

