// ArticleDoc.h : interface of the CReportDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ReportDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_)
#define AFX_ReportDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxrich.h>

class CReportDoc : public CRichEditDoc
{
protected: // create from serialization only
	CReportDoc();
	DECLARE_DYNCREATE(CReportDoc)
	

// Attributes
public:
// Implementation
public:
	    
	virtual ~CReportDoc();
	void InitFonts();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;

protected:

// Generated message  functions
protected:
	//{{AFX_MSG(CReportDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ArticleDoc_H__553CC268_C720_11D2_A6E4_A290D9000000__INCLUDED_)
