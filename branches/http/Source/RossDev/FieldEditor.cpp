// FieldEditor.cpp : implementation file
//

#include "stdafx.h"
#include "atlbase.h"
#include "FieldEditor.h"
#include "SignatEditor.h"
#include "InputBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFieldEditor dialog


CFieldEditor::CFieldEditor(CRossDoc* pRossDoc,
						   BYTE FieldNo,
						   CWnd* pParent /*=NULL*/)
	: CDialog(CFieldEditor::IDD, pParent),
	  m_pRossDoc(pRossDoc),
	  m_FieldNo(FieldNo)

{
	//{{AFX_DATA_INIT(CFieldEditor)
	m_FieldStr = _T("");
	m_bApplicToActant = FALSE;
	m_OrderIdStr = _T("");
	//}}AFX_DATA_INIT
}


void CFieldEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldEditor)
	DDX_Text(pDX, IDC_EDIT1, m_FieldStr);
	DDV_MaxChars(pDX, m_FieldStr, 100);
	DDX_Check(pDX, IDC_CHECK3, m_bApplicToActant);
	DDX_Radio(pDX, IDC_RADIO1, m_frOne);
	DDX_Radio(pDX, IDC_RADIO2, m_frMany);
	DDX_Radio(pDX, IDC_RADIO3, m_frFormula);
	DDX_Text(pDX, IDC_ORDERID_EDT, m_OrderIdStr);
	DDX_Control(pDX, IDC_LIST1, m_Signats);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CFieldEditor, CDialog)
	//{{AFX_MSG_MAP(CFieldEditor)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_BUTTON1, OnEditSignat)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, OnHdnItemdblclickList1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldEditor message handlers
const size_t IdColumn = 0;
const size_t OrdinalColumn = 1;
const size_t SignatureColumn = 2;
const size_t NameColumn = 3;

BOOL CFieldEditor::InitItem(int ItemNo) 
{
	if (ItemNo >= m_Signats.GetItemCount())
		m_Signats.InsertItem(ItemNo, "");

	const CSignat& S = GetRoss()->Fields[m_FieldNo].m_Signats[ItemNo];
	m_Signats.SetItem(ItemNo,IdColumn,LVIF_TEXT, Format("%i", S.SignatId).c_str(), 0,0,0,0);
	m_Signats.SetItem(ItemNo,OrdinalColumn,LVIF_TEXT, Format("%i", S.OrderNo).c_str(), 0,0,0,0);
	m_Signats.SetItem(ItemNo,SignatureColumn,LVIF_TEXT, S.FormatStr, 0,0,0,0);
	m_Signats.SetItem(ItemNo,NameColumn,LVIF_TEXT, S.FormatName, 0,0,0,0);
	return TRUE;
};

BOOL CFieldEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_FieldStr = GetRoss()->Fields[m_FieldNo].FieldStr;;
	m_bApplicToActant = GetRoss()->Fields[m_FieldNo].IsApplicToActant;
	m_OrderIdStr.Format("%u", GetRoss()->Fields[m_FieldNo].OrderId);
	m_Signats.InsertColumn(IdColumn,"Id",LVCFMT_LEFT, 40);
	m_Signats.InsertColumn(OrdinalColumn,"Ordinal",LVCFMT_LEFT, 50);
	m_Signats.InsertColumn(SignatureColumn,"Signature",LVCFMT_LEFT, 250);
	m_Signats.InsertColumn(NameColumn,"Name",LVCFMT_LEFT, 120);
	m_Signats.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	if (GetRoss()->Fields[m_FieldNo].TypeRes == frOne)
	{
		m_frOne = 0;
		CheckRadioButton(IDC_RADIO1,IDC_RADIO3,IDC_RADIO1);		
	};

    if (GetRoss()->Fields[m_FieldNo].TypeRes == frMany)
	{
		m_frMany = 0;
		CheckRadioButton(IDC_RADIO1,IDC_RADIO3,IDC_RADIO2);		
	};

    if (GetRoss()->Fields[m_FieldNo].TypeRes == frFormula)
	{
		m_frFormula = 0;
		CheckRadioButton(IDC_RADIO1,IDC_RADIO3,IDC_RADIO3);		
	};
	

	try {
		for (size_t i=0; i < GetRoss()->Fields[m_FieldNo].m_Signats.size(); i++)
		{
			InitItem(i);
		};
	}
	catch (...)
	{
		return FALSE;
	};

	
	UpdateData (FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFieldEditor::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	m_frOne = 1;
	m_frMany = 0;
	m_frFormula = 1;	
	UpdateData (FALSE);
}


void CFieldEditor::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	m_frOne = 0;
	m_frMany = 1;
	m_frFormula = 1;
	UpdateData (FALSE);
}

void CFieldEditor::OnRadio3() 
{
	// TODO: Add your control notification handler code here
	m_frOne = 1;
	m_frMany = 1;
	m_frFormula = 0;
	UpdateData (FALSE);
	
}

void CFieldEditor::OnOK() 
{
	UpdateData (TRUE);
	CField&  F = GetRoss()->Fields[m_FieldNo];

	if (m_FieldStr.GetLength() < FieldStrLen)
		strcpy (F.FieldStr, (const char*) m_FieldStr);
	else
	{
		ErrorMessage ("Field is too long");
		return;
	}

	if (!m_frOne)
		F.TypeRes = frOne;

	if (!m_frMany)
		F.TypeRes = frMany;

	if (!m_frFormula)
		F.TypeRes = frFormula;

	F.IsApplicToActant =  m_bApplicToActant ? TRUE : FALSE;
	F.OrderId =  atoi (m_OrderIdStr);
	m_pRossDoc->SetModifiedFlag(TRUE);	
	CDialog::OnOK();
}

// change
void CFieldEditor::OnEditSignat() 
{
	POSITION pos = m_Signats.GetFirstSelectedItemPosition();
	if (!pos)
		return;

	int ItemNo = m_Signats.GetNextSelectedItem(pos);

	try {
			CSignat& S = GetRoss()->Fields[m_FieldNo].m_Signats[ItemNo];
			CSignatEditor C (S.FormatStr, S.FormatName, S.OrderNo, m_pRossDoc);
			if (C.DoModal() != IDOK) return;
			
			if (C.m_DomainString.GetLength() > MaxFormatStrLen)
			{
				ErrorMessage("Format name is too long!");
				return;
			}
			if (C.m_Name.GetLength() > MaxFormatNameLen)
			{
				ErrorMessage("Format is too long!");
				return;
			}
			strcpy(S.FormatStr, (const char*)C.m_DomainString);
			strcpy(S.FormatName, (const char*)C.m_Name);
			S.OrderNo = C.m_OrderNo;
			InitItem(ItemNo);
		}
	catch (...)
	{
		ErrorMessage("An error occurred!");
		return;
	};

}

bool CFieldEditor::AddNewSignat() 
{
	CSignat S;
	S.SignatId = 0;
	S.OrderNo = 0;
	strcpy(S.FormatName, "no_name");
	CField& F =  GetRoss()->Fields[m_FieldNo];

	for (size_t i=0; i< F.m_Signats.size(); i++)
		if (F.m_Signats[i].SignatId > S.SignatId) 
			S.SignatId = F.m_Signats[i].SignatId;

	S.FormatStr[0] = 0;
    S.SignatId ++;
	F.m_Signats.push_back(S);
	return true;
}

// ADD NEW LINE
void CFieldEditor::OnButton2() 
{
	AddNewSignat();
	BYTE Count = GetRoss()->Fields[m_FieldNo].m_Signats.size();
	InitItem(Count-1);
	m_Signats.SetItem(Count-1,  0, LVIF_STATE, 0,0, LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED, 0, 0 );
	OnEditSignat();
	const CSignat& S = GetRoss()->Fields[m_FieldNo].m_Signats.back();
	string s = S.FormatStr;
	if (s.empty())
	{
		ErrorMessage("No domens in signature");
		GetRoss()->Fields[m_FieldNo].m_Signats.pop_back();
		m_Signats.DeleteItem(Count-1);
	};
	
}


void CFieldEditor::OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	OnEditSignat();	
	*pResult = 0;
}
