#if !defined(AFX_FIELDLIST_H__09362338_D2D1_11D2_A6F6_7C4D82000000__INCLUDED_)
#define AFX_FIELDLIST_H__09362338_D2D1_11D2_A6F6_7C4D82000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FieldList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFieldList dialog
#include "ArticleView.h"
class CRossDoc;

class CFieldList : public CDialog
{
// Construction
public:
    bool m_SelectProhibited;
	vector<CRossDevTextField>& m_Result;

	CFieldList(const CRossDoc* pRossDoc, 
				vector<CRossDevTextField>& Result, 
				CWnd* pParent = NULL);   // standard constructor
	const CRossDoc* m_pRossDoc;
	const CDictionary* GetRoss ();

// Dialog Data
	//{{AFX_DATA(CFieldList)
	enum { IDD = IDD_FIELDLIST_DIALOG };
	CListBox	m_FieldList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CFieldList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkFieldlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDLIST_H__09362338_D2D1_11D2_A6F6_7C4D82000000__INCLUDED_)
