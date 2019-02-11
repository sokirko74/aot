#if !defined(AFX_FORMULAEDITOR_H__BCDB4226_D6D6_11D2_A6FE_047834000000__INCLUDED_)
#define AFX_FORMULAEDITOR_H__BCDB4226_D6D6_11D2_A6FE_047834000000__INCLUDED_

#include "GridCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormulaEditor.h : header file

//

/////////////////////////////////////////////////////////////////////////////
// CFormulaEditor dialog

class CFormulaEditor : public CDialog
{
	int					m_RowsCount;
	int					m_ColumnsCount;
	CRossDevTextField&			m_Field;
	vector<TCortege>&	m_OldValue;
	vector<TCortege>&	m_NewValue;
	CRossDoc*			m_pRossDoc;


	size_t	GetEmptyCellNo (size_t LineNo);
	void	GetSelectedItem(int& nCol, int& nRow) const;
	bool	SetRowNumbers();
	CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}

public:
    CFormulaEditor(CRossDoc* pRossDoc,
	 	           vector<TCortege>& OldValue,
		           vector<TCortege>& NewValue,
			       CRossDevTextField& F,
		           CWnd* pParent = NULL);   // standard constructor

    
	afx_msg void OnFormulaEditorValue();
	
	

// Dialog Data
	//{{AFX_DATA(CFormulaEditor)
	enum { IDD = IDD_FORMULAEDITOR };
	CGridCtrl m_Grid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormulaEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CFormulaEditor)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMULAEDITOR_H__BCDB4226_D6D6_11D2_A6FE_047834000000__INCLUDED_)
