// CntrItem.h : interface of the CTestCntrItem class
//

#if !defined(AFX_CNTRITEM_H__B1CD761F_0C57_11D3_A764_500526000000__INCLUDED_)
#define AFX_CNTRITEM_H__B1CD761F_0C57_11D3_A764_500526000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReportDoc;
class CTestView;

class CTestCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CTestCntrItem)

// Constructors
public:
	CTestCntrItem(REOBJECT* preo = NULL, CReportDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CReportDoc* GetDocument()
		{ return (CReportDoc*)CRichEditCntrItem::GetDocument(); }
	/*CTestView* GetActiveView()
		{ return (CTestView*)CRichEditCntrItem::GetActiveView(); }*/

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestCntrItem)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	~CTestCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__B1CD761F_0C57_11D3_A764_500526000000__INCLUDED_)
