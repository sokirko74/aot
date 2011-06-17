// graphtest3Doc.h : interface of the CHierarchyDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHTEST3DOC_H__6DD15253_FD3E_11D2_8719_B8D0AF764170__INCLUDED_)
#define AFX_GRAPHTEST3DOC_H__6DD15253_FD3E_11D2_8719_B8D0AF764170__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CHierarchyView;


class CHierarchyDoc : public CDocument,
                      public CHierarchyHolder
{
public: // create from serialization only
  CHierarchyDoc() {};	
  DECLARE_DYNCREATE(CHierarchyDoc)

// Attributes
public:
  
// Operations
public:

	//LONG GetNodeNoByItemNo (LONG ItemNo);
	//LONG GetTopmostWork (LONG ItemNo);
    CHierarchyView* GetView()
    {
     	POSITION pos = GetFirstViewPosition();
		return (CHierarchyView*)GetNextView( pos );
	};
	virtual void CreateNode(const char* name, POINT p, bool WithoutView);
	virtual void CreateRelation(int node1, int node2, bool bWork, bool WithoutView);
	
    CString  GetNewNode(CString Nodes);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHierarchyDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHierarchyDoc();
	virtual BOOL DoFileSave();
    bool OpenHierarchy(CRossDoc* pRossDoc,CHierarchyEnum Type);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message  functions
protected:
	//{{AFX_MSG(CHierarchyDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHTEST3DOC_H__6DD15253_FD3E_11D2_8719_B8D0AF764170__INCLUDED_)
