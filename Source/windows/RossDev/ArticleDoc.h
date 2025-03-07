#pragma once
#include "StdAfx.h"
#include "ArticleView.h"
#include "dicts/StructDictLib/TempArticle.h"



class CArticleDoc : public CDocument
{
protected: // create from serialization only
	CArticleDoc();
	DECLARE_DYNCREATE(CArticleDoc)
	

// Attributes
public:
   CRossDoc*   m_pRossDoc;
   std::vector<CString> m_Lines;

   uint16_t  m_UnitNo;
   CTempArticle m_Article;
   bool m_WasSaved ;
   

   

// Operations
public:
   CArticleView* GetArticleView()
	{
     	POSITION pos = GetFirstViewPosition();
		return (CArticleView*)GetNextView( pos );
	};
   


   BOOL    OpenArticle (uint16_t UnitNo, CRossDoc* pRossDoc);
   CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}
           bool Markout ();
   virtual void SetTitle(LPCTSTR lpszTitle);
   virtual BOOL SaveModified(); 
   virtual BOOL DoFileSave();
   virtual BOOL IsModified();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArticleDoc)
	public:
	virtual BOOL OnNewDocument();
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

