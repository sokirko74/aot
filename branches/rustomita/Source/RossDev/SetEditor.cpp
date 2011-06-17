// SetEditor.cpp : implementation file
//

#include "stdafx.h"
#include "SetEditor.h"
#include "ValueDlg.h"
#include "../common/utilit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetEditor dialog


CSetEditor::CSetEditor(CRossDoc* pRossDoc,
					   vector<TCortege>& OldValue,
		               vector<TCortege>& NewValue,
			           CRossDevTextField& F,
					   CWnd* pParent /*=NULL*/)
	: CDialog(CSetEditor::IDD, pParent),
	  m_OldValue (OldValue),
	  m_NewValue (NewValue),
	  m_Field (F),
	  m_pRossDoc(pRossDoc)
{
	//{{AFX_DATA_INIT(CSetEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetEditor)
	DDX_Control(pDX, IDC_SETEDITOR_LIST, m_ListBox);
	//}}AFX_DATA_MAP
}




BEGIN_MESSAGE_MAP(CSetEditor, CDialog)
	//{{AFX_MSG_MAP(CSetEditor)
	ON_BN_CLICKED(IDC_SETEDITOR_DEL, OnSeteditorDel)
	ON_BN_CLICKED(IDC_SETEDITOR_ADD, OnSeteditorAdd)
	ON_BN_CLICKED(IDC_SETEDITOR_VALUE, OnSeteditorValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetEditor message handlers

BOOL CSetEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();


    SetWindowText(CString("Значения поля ") + ConstructFldName(GetRoss(), m_Field.FieldNo, m_Field.LeafId, m_Field.BracketLeafId));

	// установка шрифта
	CFont* F = m_ListBox.GetFont();
	LOGFONT LogFont;
	F->GetLogFont(&LogFont);
	LogFont.lfHeight = -20;
	CFont G;
	G.CreateFontIndirect(&LogFont);
	m_ListBox.SetFont(&G);
	

	for (size_t i=0; i < m_OldValue.size(); i++)
	{
		CString Q = WriteToString(GetRoss(), GetRoss()->Fields[m_OldValue[i].m_FieldNo].m_Signats[m_OldValue[i].GetSignatNo()].sFrmt, m_OldValue[i]).c_str();
		m_ListBox.AddString (Q);
	};

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetEditor::OnSeteditorDel() 
{
	// TODO: Add your control notification handler code here
	int nPos = m_ListBox.GetCurSel();
	if (nPos != LB_ERR)
	{
	  m_ListBox.DeleteString(nPos);
	};
	
}

void CSetEditor::OnSeteditorAdd() 
{
	// TODO: Add your control notification handler code here
	TCortege OldValue;
	TCortege NewValue;

	OldValue.m_FieldNo = m_Field.FieldNo;
	OldValue.m_LeafId = m_Field.LeafId;
	OldValue.m_BracketLeafId = m_Field.BracketLeafId;
	OldValue.m_LevelId = 0;
	OldValue.m_SignatNo = 0;

	for (size_t i = 0; i < MaxNumDom; i++)
		OldValue.m_DomItemNos[i] = -1;



	CValueDlg Dlg (m_pRossDoc, OldValue, NewValue);

	try 
	{
	  if (Dlg.DoModal() != IDOK) return;
	}
	catch (...)
	{
		return;
	};

	CString Q = WriteToString(GetRoss(), GetRoss()->Fields[NewValue.m_FieldNo].m_Signats[NewValue.GetSignatNo()].sFrmt, NewValue).c_str();
	m_ListBox.AddString (Q);
}

void CSetEditor::OnSeteditorValue() 
{
	// TODO: Add your control notification handler code here
    int nPos = m_ListBox.GetCurSel(); 
	if (nPos != LB_ERR)
	{
		CString S;
		m_ListBox.GetText (nPos, S);
		S = ConstructFldName(GetRoss(), m_Field.FieldNo, m_Field.LeafId, m_Field.BracketLeafId) + CString(" = ") + S; 
		CTempArticle A;
		A.m_pRoss = GetRoss();
		A.SetArticleStr( (const char*)S );

		TCortege OldValue = A.GetCortege(0);
     	TCortege NewValue;
  		CValueDlg Dlg (m_pRossDoc, OldValue, NewValue);
		try 
		{
		  if (Dlg.DoModal() != IDOK) return;
		}
		catch (...)
		{
			return;
		};

		CString Q = WriteToString(GetRoss(), GetRoss()->Fields[NewValue.m_FieldNo].m_Signats[NewValue.GetSignatNo()].sFrmt, NewValue).c_str();
		m_ListBox.DeleteString(nPos);
		m_ListBox.InsertString (nPos, Q);
	};
}

void CSetEditor::OnOK() 
{
	// TODO: Add extra validation here
	CString S = ConstructFldName(GetRoss(), m_Field.FieldNo, m_Field.LeafId, m_Field.BracketLeafId) + CString(" = ");
	for (size_t i=0;  i < m_ListBox.GetCount(); i ++)
	{
		CString Q;
		m_ListBox.GetText (i, Q);
		S =  S + Q + CString ("\r\n"); 
	};

	try  {
		CTempArticle A;
		A.m_pRoss =  GetRoss();
		A.SetArticleStr(   (const char*)S );
		size_t Count = A.GetCortegesSize();
		m_NewValue.clear();
		for (int i=0;  i < Count; i++)
			m_NewValue.push_back(A.GetCortege(i));
	}
	catch (...)
	{
		AfxMessageBox ("Cannot set article");
	};

	CDialog::OnOK();
}

