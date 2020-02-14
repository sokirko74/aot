// ArticleDoc.h : interface of the CArticleDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ArticleDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_)
#define AFX_ArticleDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "ArticleView.h"       // main symbols
#include "../StructDictLib/TempArticle.h"       // main symbols



class CArticleDoc : public CDocument
{
protected: // create from serialization only
	CArticleDoc();
	DECLARE_DYNCREATE(CArticleDoc)
	

// Attributes
public:
   CWinThread* m_MessageKiller;
   CRossDoc*   m_pRossDoc;
   vector<CString> m_Lines;

   WORD  m_UnitNo;
   CTempArticle m_Article;
   bool m_WasSaved ;
   

   

// Operations
public:
   CArticleView* GetArticleView()
	{
     	POSITION pos = GetFirstViewPosition();
		return (CArticleView*)GetNextView( pos );
	};
   string GetArticleErrorStr() const;


   BOOL    OpenArticle (WORD UnitNo, CRossDoc* pRossDoc);
   CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}
           bool Markout ();
		   bool AddCortegeToVector (vector<TCortege10>& L, CRossDevTextField& F);
   virtual void SetTitle(LPCTSTR lpszTitle);
   virtual BOOL SaveModified(); 
   virtual BOOL DoFileSave();
   virtual BOOL IsModified();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArticleDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArticleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message  functions
protected:
	//{{AFX_MSG(CArticleDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ArticleDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_)
