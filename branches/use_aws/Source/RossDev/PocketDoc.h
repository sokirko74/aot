// ArticleDoc.h : interface of the CPocketDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PocketDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_)
#define AFX_PocketDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPocketDoc : public CDocument
{
protected: // create from serialization only
	CPocketDoc();
	DECLARE_DYNCREATE(CPocketDoc)
	

// Attributes
public:
// Implementation
public:
	virtual ~CPocketDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message  functions
protected:
	//{{AFX_MSG(CPocketDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ArticleDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_)
