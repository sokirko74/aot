// graphtest3Doc.h : interface of the CSemanticStrDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHTEST4DOC_H__6DD15253_FD3E_11D2_8719_B8D0AF764170__INCLUDED_)
#define AFX_GRAPHTEST4DOC_H__6DD15253_FD3E_11D2_8719_B8D0AF764170__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSemanticStrView;
#include "RossDoc.h"

class CSemanticStrDoc : public CDocument
{
protected: // create from serialization only
  CSemanticStrDoc() 
  {
	  m_bBusy = false;
  };	
  DECLARE_DYNCREATE(CSemanticStrDoc)

	

// Operations
public:
	bool m_bBusy;


    CSemanticStrView* GetView()
    {
     	POSITION pos = GetFirstViewPosition();
		return (CSemanticStrView*)GetNextView( pos );
	};
	
    
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL OnNewDocument();
protected:

// Generated message  functions
protected:
	//{{AFX_MSG(CSemanticStrDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	
	//}}AFX_MSG
public : 
	virtual void SetTitle(LPCTSTR lpszTitle);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHTEST3DOC_H__6DD15253_FD3E_11D2_8719_B8D0AF764170__INCLUDED_)
