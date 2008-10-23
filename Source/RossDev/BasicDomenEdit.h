#if !defined(AFX_BASICDOMENEDIT_H__74240403_E11D_11D2_A71A_D2882A000000__INCLUDED_)
#define AFX_BASICDOMENEDIT_H__74240403_E11D_11D2_A71A_D2882A000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasicDomainEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBasicDomainEdit dialog

class CBasicDomainEdit : public CDialog
{
// Construction
public:
    CBasicDomainEdit( CRossDoc* pRossDoc,
					  BYTE DomNo, 
					  bool EditMode,
					  vector<CString>* pHiddenItems = NULL,
					  LONG* pItemNo = NULL, 
					  CWnd* pParent = NULL);


   CRossDoc* m_pRossDoc;
   CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}
   vector<CString>* m_pHiddenItems;
   LONG* m_pItemNo;
   
   BYTE m_DomNo;
   bool  m_EditMode;

   void AddItem(CString Item); 
   void UpdateConsts();
   int GetSelectedItemPos ()
	{
 		POSITION pos = m_List.GetFirstSelectedItemPosition();

  		if (pos == NULL)
			{ ::MessageBox(0, "В списке не выделено ни одного элемента", "Message Box", MB_OK);
			  return -1;};

		return m_List.GetNextSelectedItem(pos);
	};
// Dialog Data
	//{{AFX_DATA(CBasicDomainEdit)
	enum { IDD = IDD_BASICDOMENDLG };
	CButton	m_SaveAs;
	CButton	m_DelButton;
	CButton	m_AddButton;
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicDomainEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CBasicDomainEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteBtn();
	virtual void OnOK();
	afx_msg void OnButton1();
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDeleteAllItems();
	afx_msg void OnBnClickedChangeColor();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASICDOMENEDIT_H__74240403_E11D_11D2_A71A_D2882A000000__INCLUDED_)
