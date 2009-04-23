// FieldsListEditor.cpp : implementation file
//

#include "stdafx.h"
#include "FieldsListEditor.h"
#include "FieldEditor.h"
#include "resource.h"
#include "InputBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFieldsListEditor dialog


CFieldsListEditor::CFieldsListEditor(CRossDoc* pRossDoc,
						 CWnd* pParent /*=NULL*/)
	: CDialog(CFieldsListEditor::IDD, pParent),
	  m_pRossDoc(pRossDoc)

{
	//{{AFX_DATA_INIT(CFieldsListEditor)
	//}}AFX_DATA_INIT
}


void CFieldsListEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldsListEditor)
	DDX_Control(pDX, IDC_LISTFIELD, m_FieldList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFieldsListEditor, CDialog)
	//{{AFX_MSG_MAP(CFieldsListEditor)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldsListEditor message handlers

void CFieldsListEditor::Update() 
{
	m_FieldList.ResetContent();

	for (size_t i =0; i < GetRoss()->Fields.size(); i++)
	{
		int nPos = m_FieldList.AddString(GetRoss()->Fields[i].FieldStr);
		m_FieldList.SetItemData(nPos, i);
	};
	

};
BOOL CFieldsListEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Update();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CFieldsListEditor::OnButton1() 
{
	// TODO: Add your control notification handler code here
   int nPos = m_FieldList.GetCurSel();
   if (nPos == LB_ERR)
	{
		AfxMessageBox ("No object selected!");  
		return;
	};
   
   CFieldEditor dlg (m_pRossDoc, m_FieldList.GetItemData(nPos));	
   dlg.DoModal();
   m_pRossDoc->SetModifiedFlag(TRUE);
}

// добавить
void CFieldsListEditor::OnAddButton() 
{
	// TODO: Add your control notification handler code here
   char ItemStr[100]; 
   ItemStr[0] = 0;
   if (   !InputBox("Input a new field:", ItemStr, 100) 
  	    || (strlen (ItemStr) == 0))
		return;

   if (GetRoss()->GetFieldNoByFieldStr(ItemStr) != ErrUChar)
   {
	   AfxMessageBox ("This field already exists!");
	   return;
   };

   GetRoss()->AddField(ItemStr);
   Update();

}
