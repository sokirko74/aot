#if !defined(AFX_POCKETFORM_H__0B903003_D862_11D2_A701_20B37E000000__INCLUDED_)
#define AFX_POCKETFORM_H__0B903003_D862_11D2_A701_20B37E000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PocketForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPocketForm dialog
struct CRossPocketItem 
{
	WORD	 m_UnitNo;
	CRossDoc* m_pRossDoc;
	CRossPocketItem()
	{
	   m_UnitNo = ErrUnitNo;
	   m_pRossDoc = 0;;
    };
	CRossPocketItem(WORD	 UnitNo, CRossDoc* pRossDoc)
	{
	   m_UnitNo = UnitNo;
	   m_pRossDoc = pRossDoc;
    };
};

class CPocketForm : public CFormView
{
// Construction
public:
	CPocketForm();   // standard constructor
	DECLARE_DYNCREATE(CPocketForm)

	vector<CRossPocketItem> m_PocketItems;
	CString m_Title;
	bool GetSelectedUnitNo (WORD& UnitNo)
	{
 		POSITION pos = m_WordList.GetFirstSelectedItemPosition();

  		if (pos == NULL)
			{ ::MessageBox(0, "В списке не выделено ни одного словарного входа", "Message Box", MB_OK);
			  return false;};

		UnitNo = m_WordList.GetNextSelectedItem(pos);

		return true;

	};
   int Open(CString Title);
	

// Dialog Data
	//{{AFX_DATA(CPocketForm)
	enum { IDD = IDD_POCKET_FORM };
	CListCtrl	m_WordList;
	CString	m_UnitsSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPocketForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CPocketForm)
	afx_msg void OnGetdispinfoList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButton1();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnDblclkWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

extern int OpenPocket(const vector<CRossPocketItem>& UnitNos, CString    Title);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POCKETFORM_H__0B903003_D862_11D2_A701_20B37E000000__INCLUDED_)
