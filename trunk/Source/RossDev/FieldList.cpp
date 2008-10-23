// FieldList.cpp : implementation file
//

#include "stdafx.h"
#include "FieldList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFieldList dialog


CFieldList::CFieldList(const CRossDoc* pRossDoc, 
						vector<CRossDevTextField>& Result,
						CWnd* pParent /*=NULL*/)
	: CDialog(CFieldList::IDD, pParent), 
	  m_Result(Result)
{
	m_pRossDoc = pRossDoc;
	
	
	//{{AFX_DATA_INIT(CFieldList)
	//}}AFX_DATA_INIT
}
const CDictionary* CFieldList::GetRoss () 
{
	return m_pRossDoc->GetRoss();
}


void CFieldList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldList)
	DDX_Control(pDX, IDC_FIELDLIST, m_FieldList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFieldList, CDialog)
	//{{AFX_MSG_MAP(CFieldList)
	ON_WM_COMPAREITEM()
	ON_LBN_DBLCLK(IDC_FIELDLIST, OnDblclkFieldlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldList message handlers



BOOL CFieldList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	BYTE FieldsSize =  GetRoss()->Fields.size();
	for (size_t i= 0; i < FieldsSize; i++)
	{
		CRossDevTextField F(i, 0,0);
		for (;  F.LeafId < 8; F.LeafId++)
			if (   GetRoss()->Fields[i].IsApplicToActant
				|| (F.LeafId == 0)
				)
			{
				int weight = i*10 + F.LeafId;
				int j = m_FieldList.GetCount();
				for (; j > 0; j--)
					if (weight > m_FieldList.GetItemData(j-1))
						break;
				m_FieldList.InsertString(j, ConstructFldName (GetRoss(), i, F.LeafId, F.BracketLeafId));
				m_FieldList.SetItemData (j, weight);
			}; 

	};





	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}





void CFieldList::OnOK() 
{
	// TODO: Add extra validation here
	m_Result.clear();
	for (size_t i=0; i< m_FieldList.GetCount(); i++)
     if (m_FieldList.GetSel(i) != 0)
	 {
		 int j = m_FieldList.GetItemData(i);
		 m_Result.push_back (CRossDevTextField ((int)(j/10), j % 10, 0));
	 };

	CDialog::OnOK();
}

void CFieldList::OnDblclkFieldlist() 
{
	CFieldList::OnOK();	
}
