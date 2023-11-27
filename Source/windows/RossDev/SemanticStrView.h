#pragma once

#include "StdAfx.h"
#include "SemanticStrDoc.h"


class  CRusSemStructure;

class CSemanticStrView : public CView
{
	CTclTkWrapper& m_TclInterp;
	void SwitchMainTkWindow();
protected: 
	DECLARE_DYNCREATE(CSemanticStrView)

// Attributes
private: 
	CString					m_Title;
	CMenu					m_Menu;	
public:
	CTopTkWindow m_TopTKWindow;
	CSemanticStrView();
	~CSemanticStrView();
	
	CSemanticStrDoc*    GetDoc();
	void				BuildTclGraph(std::string s_utf8);
	void				ShowGraph();
	void				OpenAllRosses();
	void				PasteClipboard();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSemanticStrView)
	public:
	virtual void OnDraw(CDC* pDC);
	//virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, uint32_t dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
private:
	void		OpenRossDocIfNeeded(DictTypeEnum RossType);
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
	afx_msg BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	//}}AFX_MSG

protected:
	afx_msg void OnBackBtn();
	afx_msg void OnForwardBtn();

	DECLARE_MESSAGE_MAP()
};


