#if !defined(AFX_ARTICLEVIEW_H__DD981AC7_CBCD_11D2_A6EB_AC559A000000__INCLUDED_)
#define AFX_ARTICLEVIEW_H__DD981AC7_CBCD_11D2_A6EB_AC559A000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "SizeFormView.h "
#include "SyntaxColorizer.h"

#include  "FldScroll.h"
#include "afxcmn.h"

class CMyEditCtrl : public CRichEditCtrl
{
protected:
	DECLARE_DYNCREATE(CMyEditCtrl)
	afx_msg void OnLButtonDblClk( UINT nFlags,  CPoint point );
	DECLARE_MESSAGE_MAP()
};


class CArticleView : public CSizeFormView
{
protected:
	CArticleView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CArticleView)
    CFont m_TextFont;
	CFont m_HyperTextFont;


	void parse();
	void parse2();
// Form Data
public:
	//{{AFX_DATA(CArticleView)
	enum { IDD = IDD_ARTICLEFORM };
	CFldScroll	    m_FldScroll;
	//CRossDoc*       m_pRossDoc;
	//}}AFX_DATA

// Attributes
public:
	enum   ActionType {fnAdd, fnInsert};
	LOGFONT m_StandardLogFont;

	void InsertLine (int LineNo, const CString& S);
	void Clear ()
	{
		m_RichEdit.SetWindowText("");
	};
	void SelectLine ( long LineNo );
	void SetText(CString S);
	void DeleteLine(size_t i);

	CString GetText() const
	{
		CString A;
		m_RichEdit.GetWindowText(A);
		return  A;
	};
   
	

// Operations
public:
	void   SetFocusToField (int nPos);
	CDictionary* GetRoss ();
	CRossDoc*  GetRossDoc ();
	void   	GotoLastField();
	void   	AddOrInsertRossField (int nPos, ActionType fnAction);
	bool   	GetFieldList (vector<CRossDevTextField>& Result, vector<CRossDevTextField>& Prohibited);
	void   	EditValue(int nPos);
	void   	EditOneValue(CRossDevTextField& F);
	void   	EditFormulaValue (int nPos, char TypeRes);
	void	WriteToEdit (vector<TCortege10>& L, size_t nPos);
	void	DelField(size_t nPos);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArticleView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CArticleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message  functions
	//{{AFX_MSG(CArticleView)
	afx_msg void OnArticleformClose();
	afx_msg void OnArticleviewMarkup();
	afx_msg void OnArticleviewTextbtn();
	afx_msg void OnAddButton();
	afx_msg void OnArticleViewDelField();
	afx_msg void OnArticleFormValue();
	afx_msg void OnChangeRichedit1();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCut();
	afx_msg void OnEditUndo();
	

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CMyEditCtrl m_RichEdit;
	CSyntaxColorizer m_Colorizer;

};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARTICLEVIEW_H__DD981AC7_CBCD_11D2_A6EB_AC559A000000__INCLUDED_)
