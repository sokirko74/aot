#ifndef _ValueDlh_h
#define _ValueDlh_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include  "RossDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CValueDlg dialog
class CRossDoc;
class CValueDlg : public CDialog
{
// Construction
public:
	CValueDlg(CRossDoc  *RossDoc,
		      TCortege& OldValue, 
		      TCortege& NewValue,
		      CWnd* pParent = NULL);   // standard constructor

	CValueDlg(CRossDoc  *RossDoc,
		      TCortege& OldValue, 
		      TCortege& NewValue,
			  const CSignat*  pSignat, 
		      CWnd* pParent = NULL);   // standard constructor
   ~CValueDlg();
// Dialog Data
	//{{AFX_DATA(CValueDlg)
	enum { IDD = IDD_VALUEDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
   
   TCortege& m_OldValue; 
   TCortege& m_NewValue;
   //IDictionaryPtr m_pIRossForSyntax;
   CRossDoc  *m_pRossDoc;
   CDictionary* GetRoss () {return m_pRossDoc->GetRoss();}
   BYTE             m_FieldNo;
   CWnd*            m_pSignatGroup;
   BYTE             m_CurrFunDimen;
   CComboBox        m_Lists[MaxNumDom];
   CEdit            m_Edits[MaxNumDom];
   CStatic          m_Labels[MaxNumDom];
   vector<CButton*> m_SignatsBtns;
   bool             m_bOneFormat;
   BYTE             m_SignatNo;
   const CSignat*   m_pSignat;


   void            OnChangeSignatNo(BYTE NewSignatNo);
   void            DrawControls ();
   void            ClearControls ();
   BYTE            GetSignatDomNo(BYTE i)
   {
	   return (!m_bOneFormat) ? 
		   GetRoss()->Fields[m_FieldNo].m_Signats[m_NewValue.GetSignatNo()].Doms[i]
			  : m_pSignat->Doms[i];};

   BYTE            GetSignatDomsCount()
   {
	   return (!m_bOneFormat) ? 
		   GetRoss()->Fields[m_FieldNo].m_Signats[m_NewValue.GetSignatNo()].Doms.size()
			  : m_pSignat->Doms.size();};
   bool SetNewValue();
   

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValueDlg)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message  functions
	//{{AFX_MSG(CValueDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
