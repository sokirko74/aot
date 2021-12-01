#if !defined(AFX_REPORTVIEW_H__3C928693_4DC6_11D5_AC09_9EB351000000__INCLUDED_)
#define AFX_REPORTVIEW_H__3C928693_4DC6_11D5_AC09_9EB351000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportView view

class CReportView : public CRichEditView
{
protected:
	CReportView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CReportView)

// Attributes
public:
	CMenu				 m_Menu;	


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CReportView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message  functions
	//{{AFX_MSG(CReportView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTVIEW_H__3C928693_4DC6_11D5_AC09_9EB351000000__INCLUDED_)
