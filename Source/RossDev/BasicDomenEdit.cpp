// BasicDomenEdit.cpp : implementation file
//

#include "stdafx.h"
#include "BasicDomenEdit.h"
#include "InputBox.h"
#include "ValueDlg.h"
#include "../common/utilit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicDomenEdit dialog


CBasicDomainEdit::CBasicDomainEdit(CRossDoc* pRossRoc,
								  BYTE DomNo, 
								  bool EditMode,
								  vector<CString>* pHiddenItems,
								  LONG* pItemNo,
								  CWnd* pParent /*=NULL*/)
	: CDialog(CBasicDomainEdit::IDD, pParent),
	  m_pRossDoc(pRossRoc),
	  m_DomNo (DomNo),
	  m_EditMode (EditMode)
	  
{
	m_pHiddenItems  = pHiddenItems;
	m_pItemNo  = pItemNo;
	//{{AFX_DATA_INIT(CBasicDomainEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBasicDomainEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasicDomainEdit)
	DDX_Control(pDX, IDC_BUTTON1, m_SaveAs);
	DDX_Control(pDX, IDC_BUTTON2, m_DelButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBasicDomainEdit, CDialog)
	//{{AFX_MSG_MAP(CBasicDomainEdit)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_BUTTON2, OnDeleteBtn)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_DELETE_ALL_ITEMS, OnBnClickedDeleteAllItems)
	//}}AFX_MSG_MAP
	
	ON_BN_CLICKED(IDC_CHANGE_COLOR, OnBnClickedChangeColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicDomainEdit message handlers

void CBasicDomainEdit::UpdateConsts() 
{

  m_List.DeleteAllItems();
  for (size_t i=0; i<GetRoss()->GetDomItemsSize(); i++)
	{
		if (GetRoss()->GetDomItemDomNo(i) == m_DomNo) 
		if (     (m_pHiddenItems == NULL)
			  || (find (m_pHiddenItems->begin(), m_pHiddenItems->end(),(const char*)GetRoss()->GetDomItemStr(i)) == m_pHiddenItems->end())
		   )
		{
		  int nPos = m_List.InsertItem(m_List.GetItemCount(), (const char*)GetRoss()->GetDomItemStr(i),0);
		  m_List.SetItemData(nPos, i);
		};
	};


};


BOOL CBasicDomainEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_List.InsertColumn( 0,"Constant", LVCFMT_LEFT, 150);

	UpdateConsts();

	if (!m_EditMode)
	{
		m_AddButton.ShowWindow(SW_HIDE);
		m_DelButton.ShowWindow(SW_HIDE);
		m_SaveAs.ShowWindow(SW_HIDE);
	};

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBasicDomainEdit::AddItem(CString Item) 
{
	int i = GetRoss()->GetItemNoByItemStr((const char*)Item, m_DomNo);
	if (i != -1)
	{
		AfxMessageBox ("Cannot add a dublicate string!");
		return;
	};

	bool  bRes = GetRoss()->InsertDomItem((const char*)Item, m_DomNo, i);
	if (!bRes)
	{
		AfxMessageBox ("Cannot insert an item!");
		return;
	}

	int nPos = m_List.InsertItem(m_List.GetItemCount(), Item,0);
	m_List.SetItemData(nPos, i);

	m_pRossDoc->SetModifiedFlag(TRUE);

};
// добавление
void CBasicDomainEdit::OnAddButton() 
{
	// TODO: Add your control notification handler code here
  try
  {
  char OutBuffer[100];
  OutBuffer[0] =0;
  if (GetRoss()->m_Domens[m_DomNo].Source == dsUnion)
  {
      if (!InputBox ("Новый домен", OutBuffer, 100))
		  return;

	  if (GetRoss()->GetDomenNoByDomStr(OutBuffer) == ErrUChar)
	  {
		   AfxMessageBox ("No domen found");
		   return;
	  };

  }
  else
  if (GetRoss()->m_Domens[m_DomNo].Source == (unsigned char)dsExpres)
  {
 	TCortege OldValue;
	TCortege NewValue;
	CSignat S; 
    
	if (!GetRoss()->BuildOneFieldFormat (S, GetRoss()->m_Domens[m_DomNo].Format, GetRoss()->m_Domens[m_DomNo].DomStr, GetRoss()->m_MaxNumDom))
	{
		ErrorMessage(GetRoss()->m_LastError);
	}

	CValueDlg Dlg (m_pRossDoc, OldValue, NewValue,  &S);

	try 
	{
	  if (Dlg.DoModal() != IDOK) return;
	}
	catch (...)
	{
		return;
	};

	CString Q = CString(WriteToString(GetRoss(), S.sFrmtWithotSpaces, NewValue).c_str());
	Q.TrimLeft();
	Q.TrimRight();
	strcpy (OutBuffer, Q);
  }
  else
    if (!InputBox ("Новое значение", OutBuffer, 100))
		return;
    AddItem(CString (OutBuffer));

  }
  catch (...)
  {
 	return;
  };
}

// удалить 
void CBasicDomainEdit::OnDeleteBtn() 
{
	size_t Pos = GetSelectedItemPos ();
	while (Pos != -1)
	{

		LONG ItemNo = m_List.GetItemData(Pos);
		vector<int>  Refs;
		for (size_t i = 0;  i<GetRoss()->_GetCortegesSize(); i++)
			for (size_t k=0; k < MaxNumDom; k++)
				if (GetCortege(GetRoss(), i).m_DomItemNos[k] == ItemNo)
				{
					Refs.push_back(i);
					break;
				}

				if (Refs.size() > 0)
				{
					CString prompt;
					prompt.Format("Константа \"%s\" упоминается в словаре %i раз. Придется удалить все эти упоминания. Продолжить?", (const char*)GetRoss()->GetDomItemStr(ItemNo), Refs.size());
					if (AfxMessageBox (prompt, MB_OKCANCEL) == IDCANCEL)
						break;
				}

				GetRoss()->DelDomItem(ItemNo);

				m_List.DeleteItem(Pos);

				m_pRossDoc->SetModifiedFlag(TRUE);

				UpdateConsts();


				m_List.SetItemState( 0,   LVIS_FOCUSED| LVIS_SELECTED , LVIS_FOCUSED|LVIS_SELECTED );
				break;

	};



}

void CBasicDomainEdit::OnOK() 
{
	// TODO: Add extra validation here
	if (m_pItemNo)
	{
		int nPos = GetSelectedItemPos ();
		if (nPos == -1) return;
		*m_pItemNo = m_List.GetItemData(nPos);
	};

	
	CDialog::OnOK();

}

// SaveAs
void CBasicDomainEdit::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CFileDialog  D (FALSE, "txt",GetRoss()->m_Domens[m_DomNo].DomStr);
	if (D.DoModal() != IDOK) return;
	FILE* fp = fopen (D.GetPathName(), "wb");
	for (size_t i = 0; i < m_List.GetItemCount(); i++)
		fprintf (fp, "%s\r\n", m_List.GetItemText(i, 0));
	fclose(fp);
}

// прочитать из файла
void CBasicDomainEdit::OnButton3() 
{
	// TODO: Add your control notification handler code here
    CFileDialog  D (TRUE, "txt", GetRoss()->m_Domens[m_DomNo].DomStr);
	if (D.DoModal() != IDOK) return;
	FILE* fp = fopen (D.GetPathName(), "rb");
	char s[100];
	while (fgets(s, 100, fp))
	{
		CString Q (s);
	    Q.TrimLeft();
		Q.TrimRight();
		AddItem(Q);

	};
	fclose(fp);
}


void CBasicDomainEdit::OnBnClickedDeleteAllItems()
{
	
	for (;;)
	{
		m_List.SetItemState( 0,   LVIS_FOCUSED| LVIS_SELECTED , LVIS_FOCUSED|LVIS_SELECTED );
		POSITION pos = m_List.GetFirstSelectedItemPosition();
  		if (pos != NULL)
			OnDeleteBtn();
		else
			break;
		
	};
		
}

void CBasicDomainEdit::OnBnClickedChangeColor()
{
	CFontDialog dlg;
    BYTE no = m_pRossDoc->m_DomParamsPtr[m_DomNo];
	COLORREF Color = 0;
	if (no != ErrUChar) 
	{
		dlg.m_cf.rgbColors = m_pRossDoc->m_DomainParams[no].Color;
		Color = dlg.m_cf.rgbColors;
	}
	if (dlg.DoModal() != IDOK) return;
	if (Color != dlg.GetColor())
	{
		if (no == ErrUChar)
		{
			m_pRossDoc->m_DomainParams.push_back(CDomainParam());
		    no = m_pRossDoc->m_DomainParams.size() - 1;
			m_pRossDoc->m_DomainParams[no].DomStr = (const char*)GetRoss()->m_Domens[m_DomNo].DomStr;
			m_pRossDoc->m_DomParamsPtr[m_DomNo] = no;
		};
        
	};

    if (Color != dlg.GetColor())
	{
		m_pRossDoc->m_DomainParams[no].Color = dlg.GetColor();
        m_pRossDoc->SetModifiedFlag(TRUE);
	};

}
