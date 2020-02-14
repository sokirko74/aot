#include "afxcmn.h"
#if !defined(AFX_IMPORTFORM_H__94D2B6A3_E2C0_11D2_A721_FC025D000000__INCLUDED_)
#define AFX_IMPORTFORM_H__94D2B6A3_E2C0_11D2_A721_FC025D000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

/////////////////////////////////////////////////////////////////////////////
// ImportForm dialog
const char g_strRegLastFile[]    = "SOFTWARE\\Dialing\\Ross\\LastImportFile"; 	 



class CProtocolCtrl : public CRichEditCtrl
{
public:
	void ShowError();
	void NextError();
	
protected:

	DECLARE_DYNCREATE(CProtocolCtrl)
	afx_msg void OnLButtonDblClk( UINT nFlags,  CPoint point );
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
};


class CSourceCtrl : public CRichEditCtrl
{
public:
	void ShowError();
protected:

	DECLARE_DYNCREATE(CSourceCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
};



class CImportForm : public CDialog
{

// Construction
public:
    CFont m_TextFont;
    CFont m_HyperTextFont;
	long  m_ErrorsCount;


	CString m_CurTitle;
	CImportForm(CRossDoc* pRossDoc,
		         CWnd* pParent = NULL);   // standard constructor

	~CImportForm();

    CRossDoc* m_pRossDoc;
	CDictionary* GetRoss () {return m_pRossDoc->GetRoss();};
	void SendInfo (CString S, long LineNo = -1);
	ImportConflictEnum GetConflictSolver() const;
	afx_msg void OnRun();


// Dialog Data
	//{{AFX_DATA(ÑImportForm)
	enum { IDD = IDD_IMPORT_DLG };
	CHotKeyCtrl	m_HotKeys;
	int		m_OverWrite;
	int		m_Append;
	int		m_Skip;
	CString	m_FileName;
	CString	m_Protocol;
	BOOL	m_SimulatingMode;
	CString	m_NumberOfArticle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ÑImportForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(ÑImportForm)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CSourceCtrl m_MainRichEdit;
	CProtocolCtrl m_ProtocolRichEdit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTFORM_H__94D2B6A3_E2C0_11D2_A721_FC025D000000__INCLUDED_)
