// SignatEditor.cpp : implementation file
//

#include "stdafx.h"
#include "rossdev.h"
#include "SignatEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SignatEditor dialog


CSignatEditor::CSignatEditor(CString DomainString, CString Name, int OrderNo, CRossDoc* pRossDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CSignatEditor::IDD, pParent)
	, m_OrderNo(0)
	, m_Name(_T(""))
{
	//{{AFX_DATA_INIT(SignatEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_DomainString = DomainString;
	m_Name = Name;
	m_OrderNo = OrderNo;
    m_pRossDoc =pRossDoc;
}


void CSignatEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SignatEditor)
	DDX_Control(pDX, IDC_SIGNAT_DOMS, m_SignatDoms);
	DDX_Control(pDX, IDC_AVAIL_DOMS, m_AvailDoms);
	DDX_Text(pDX, IDC_EDIT4, m_OrderNo);
	DDX_Text(pDX, IDC_EDIT1, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSignatEditor, CDialog)
	//{{AFX_MSG_MAP(SignatEditor)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SignatEditor message handlers

void CSignatEditor::OnAdd() 
{
	// TODO: Add your control notification handler code here
	if (m_AvailDoms.GetCurSel() == LB_ERR) return;
	long DomNo = m_AvailDoms.GetItemData(m_AvailDoms.GetCurSel());
	CString Item = (const char*)GetRoss()->m_Domens[DomNo].DomStr;
	m_SignatDoms.AddString(Item);
}

void CSignatEditor::OnDelete() 
{
	// TODO: Add your control notification handler code here
	if (m_SignatDoms.GetCurSel() == LB_ERR) return;
	CString Item;
	m_SignatDoms.GetText(m_SignatDoms.GetCurSel(), Item);
	m_SignatDoms.DeleteString(m_SignatDoms.GetCurSel());
}

BOOL CSignatEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// инициализирем набор доменов в сигнатуре
	char buffer[2000];
	assert (m_DomainString.GetLength() <  2000);
	strcpy (buffer,m_DomainString);
	char* s = strtok (buffer," ");
	while (s)
	{
		m_SignatDoms.AddString(s);
		s = strtok (NULL, " ");
	};

	// инициализируем общий набор доменов 
	for (size_t i=0; i<GetRoss()->m_Domens.size(); i++)
	{
		const CDomen&  D = GetRoss()->m_Domens[i];
		if (D.Source != dsSystem)
		{
			string DomStr =  D.DomStr;
			if (DomStr == "D_") continue;
			int nPos;
			if (!D.IsDelim)
			{
				nPos = m_AvailDoms.AddString(DomStr.c_str());
			}
			else
			{
				CString Delims;
				long j=0;
				for (; j <D.m_ItemsLength; j++)
				{
					Delims += (const char*)D.m_Items[j];
					Delims += " ";
				};
				if (j == 0) continue;
				nPos = m_AvailDoms.AddString(Delims);
			};
			m_AvailDoms.SetItemData(nPos, i);
		};
	}

	return TRUE;  
}

void CSignatEditor::OnOK() 
{
	UpdateData();
	m_DomainString = "";
    for (long i=0; i < m_SignatDoms.GetCount(); i++)
	{
		CString Item;
		m_SignatDoms.GetText(i, Item);
		m_DomainString +=  Item;
		m_DomainString +=  " ";
	};
	CDialog::OnOK();
}
