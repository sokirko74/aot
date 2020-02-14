// ValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ValueDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CValueDlg dialog


CValueDlg::CValueDlg(CRossDoc  *RossDoc,
					 TCortege& OldValue, 
		             TCortege& NewValue,
					 CWnd* pParent /*=NULL*/)
	: CDialog(CValueDlg::IDD, pParent),
	  m_pRossDoc(RossDoc),
	  m_OldValue (OldValue),
	  m_NewValue (NewValue),
	  m_FieldNo(m_OldValue.m_FieldNo)
{
	//{{AFX_DATA_INIT(CValueDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bOneFormat = false;
}

CValueDlg::CValueDlg(CRossDoc  *RossDoc,
					 TCortege& OldValue, 
		             TCortege& NewValue,
					 const CSignat* pSignat,
					 CWnd* pParent /*=NULL*/)
	: CDialog(CValueDlg::IDD, pParent),
	  m_pRossDoc(RossDoc),
	  m_OldValue (OldValue),
	  m_NewValue (NewValue)
{
	//{{AFX_DATA_INIT(CValueDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSignat = pSignat;
	m_bOneFormat = true;
}


CValueDlg::~CValueDlg()
{
	for (size_t  i=0; i <m_SignatsBtns.size(); i++)
		delete m_SignatsBtns[i];
};

void CValueDlg::DoDataExchange(CDataExchange* pDX)
{
	
	CDialog::DoDataExchange(pDX);
	if (m_SignatsBtns.size() > 0)
	{
	   int SignatNo;
	   DDX_Radio(pDX, 100, SignatNo);
       if (SignatNo != m_NewValue.GetSignatNo())
	   {
		 OnChangeSignatNo(SignatNo);
	   };
	};
	//{{AFX_DATA_MAP(CValueDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CValueDlg::OnChangeSignatNo(BYTE NewSignatNo)
{
  for (size_t i=0; i< MaxNumDom; i++)
   m_NewValue.m_DomItemNos[i] = -1;   

  ClearControls();

  m_NewValue.SetSignatNo(NewSignatNo);

  DrawControls();

};


BEGIN_MESSAGE_MAP(CValueDlg, CDialog)
	//{{AFX_MSG_MAP(CValueDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CValueDlg message handlers

void CValueDlg::ClearControls ()
{
   for (size_t i=0; i < m_CurrFunDimen; i++)
   {
	   m_Labels[i].DestroyWindow();
	   if (m_Lists[i].m_hWnd != 0)
		   m_Lists[i].DestroyWindow();
       else
	       m_Edits[i].DestroyWindow();
   };

};

const int Control_Gap = 20;
const int Control_Height = 20;
const int FontHeight =  130;


void CValueDlg::DrawControls ()
{
  // считаем арность поля
   m_CurrFunDimen = GetSignatDomsCount();

   RECT LabelRect;
   LabelRect.top = 30;
   LabelRect.left = 5;
   LabelRect.bottom = LabelRect.top + Control_Height;
   LabelRect.right = LabelRect.left +95;
   RECT ControlRect;
   ControlRect.top = LabelRect.top-3;
   ControlRect.left = LabelRect.right + 10;
   ControlRect.bottom = ControlRect.top + Control_Height;
   ControlRect.right = ControlRect.left +170;

  // создаем компоненты на каждый аргумент CWnd
   for (size_t i=0; i < m_CurrFunDimen; i++)
   {
	  BYTE DomNo = GetSignatDomNo(i);
	  m_Lists[i].m_hWnd = 0;
	  m_Edits[i].m_hWnd = 0;

	  // рисуем название
	  string LabelStr = GetRoss()->m_Domens[DomNo].DomStr;
	  if (LabelStr.substr(0,2) == "D_")
		  LabelStr.erase(0,2);

	  m_Labels[i].Create(LabelStr.c_str(), WS_VISIBLE, LabelRect, this);
/*	  CFont* Font = m_Labels[i].GetFont( );
	  LOGFONT  Fnt;
	  Fnt.lfWidth = 5;
	  Font->CreateFontIndirect (&Fnt);
	  m_Labels[i].SetFont(Font);*/
	  
	  if (    (! GetRoss()->m_Domens[DomNo].IsFree)  
			&& (DomNo != GetRoss()->LexPlusDomNo)
	 	 )  // константный
	  {
		  int DropDownCount = (m_pRossDoc->m_DomParamsPtr[DomNo] != ErrUChar) ? m_pRossDoc->m_DomainParams[m_pRossDoc->m_DomParamsPtr[DomNo]].DropDownCount : 20;
		  ControlRect.bottom += Control_Height* DropDownCount; 
		  m_Lists[i].Create(WS_VSCROLL|CBS_AUTOHSCROLL|WS_VISIBLE|CBS_SORT|CBS_DROPDOWNLIST, ControlRect, this, i);
           
		  vector<BYTE> DomNos;
		  if (GetRoss()->m_Domens[DomNo].Source == dsUnion)
			  for (size_t j=0; j<GetRoss()->m_Domens[DomNo].PartsSize; j++)
			    DomNos.push_back(GetRoss()->m_Domens[DomNo].Parts[j]);  
		  else
		       DomNos.push_back(DomNo);

		  LONG size = GetRoss()->GetDomItemsSize();
		  for (LONG k=0; k <size; k++)
			  if (find (DomNos.begin(), DomNos.end(), GetRoss()->GetDomItemDomNo(k)) != DomNos.end())
			  {
				  int nPos = m_Lists[i].AddString ((const char*)GetRoss()->GetDomItemStr(k));
				  if (m_OldValue.m_DomItemNos[i] == k)
					{
					  m_Lists[i].SetCurSel(nPos);
					}
				  m_Lists[i].SetItemData(nPos, k);
			  };
	  }
	  else
	  {
        // создается окошечко ввода
		  m_Edits[i].Create (ES_AUTOHSCROLL|WS_BORDER|WS_TABSTOP, ControlRect, this, i);
		  m_Edits[i].SetFont (&(((CRossDevApp*)AfxGetApp())->m_TextFont));
		  m_Edits[i].ShowWindow(SW_SHOW);
		  if (m_OldValue.m_DomItemNos[i] != -1)
		     m_Edits[i].SetWindowText ((const char*)GetRoss()->GetDomItemStr(m_OldValue.m_DomItemNos[i]));
		  if (i == 0)
			  m_Edits[i].SetFocus(); 

	  };

	  LabelRect.bottom +=  Control_Gap + Control_Height;
	  LabelRect.top  +=  Control_Gap + Control_Height;
	  ControlRect.bottom =  LabelRect.bottom;
	  ControlRect.top  =  LabelRect.top;
   };


};


BOOL CValueDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();


	
	// TODO: Add extra initialization here
	m_NewValue = m_OldValue;

    DrawControls ();

	if (!m_bOneFormat)
	SetWindowText(CString("One field value ") + ConstructFldName(GetRoss(), m_OldValue.m_FieldNo, m_OldValue.m_LeafId, m_OldValue.m_BracketLeafId));

    // Установка Сигнатур, если их больше одной
	m_pSignatGroup = GetDlgItem (IDC_VALUEDLG_SIGNATGROUP);
	ASSERT (m_pSignatGroup);
    

    if  (		(m_bOneFormat) 
			||  (GetRoss()->Fields[m_FieldNo].m_Signats.size() == 1)
		)

	{
	   RECT r;
	   GetWindowRect(&r);
	   SetWindowPos(&wndTopMost,r.left,r.top, 290, r.bottom - r.top, 0);
	}
    else
	 {

       RECT r;
	   m_pSignatGroup->GetWindowRect(&r);
	   int BtnGap = (r.bottom - r.top) / (GetRoss()->Fields[m_FieldNo].m_Signats.size()+1);
	   r.top = 40;
	   r.bottom =  r.top + 30;
	   r.left = 285
		   ;
	   r.right = r.left + 150;

	   m_SignatsBtns.push_back(new CButton());
	   m_SignatsBtns[0]->Create (GetRoss()->Fields[m_FieldNo].m_Signats[0].FormatName, WS_VISIBLE|WS_CHILD|BS_AUTORADIOBUTTON|WS_GROUP, r, this, 100);    


	   for (size_t i=1; i<GetRoss()->Fields[m_FieldNo].m_Signats.size(); i++)
	   {
          r.bottom += BtnGap;
		  r.top += BtnGap;
		  m_SignatsBtns.push_back(new CButton());
		  m_SignatsBtns[i]->Create (GetRoss()->Fields[m_FieldNo].m_Signats[i].FormatName, WS_VISIBLE|WS_CHILD|BS_AUTORADIOBUTTON, r, this, i+100);    
	   }

	   m_SignatsBtns[m_NewValue.GetSignatNo()]->SetCheck(1);
	}




  	
 return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CValueDlg::SetNewValue() 
{

	for (size_t i=0; i < MaxNumDom; i++)
	  m_NewValue.m_DomItemNos[i] = -1;


	for (size_t i=0; i < m_CurrFunDimen; i++)
	{
	  if (m_Lists[i].m_hWnd  != 0)
	  {
		  int nPos = m_Lists[i].GetCurSel();
		  if (nPos == LB_ERR) 
		  {
			  AfxMessageBox("Остались пустые места!");
			  return false;
		  }
		  BYTE DomNo = GetSignatDomNo(i);
		  CString S;
		  m_Lists[i].GetWindowText(S);
		  m_NewValue.m_DomItemNos[i] =  GetRoss()->GetItemNoByItemStr((const char*)S, DomNo);//m_Lists[i].GetItemData(nPos);
	  }
	  else
	  {
		BYTE DomNo = GetSignatDomNo(i);
		CString S;
		m_Edits[i].GetWindowText(S);
		if (S.IsEmpty()) 
		{
			  AfxMessageBox("No empty items are possible!");
			  return false;
		}
		S.TrimLeft();
		S.TrimRight();
        if (S.GetLength() > 99) 
		{
              AfxMessageBox (CString("The value '")+S+CString("' is too long!"));
			  return false;
		};

		m_NewValue.m_DomItemNos[i] =  GetRoss()->GetItemNoByItemStr((const char*)S, DomNo);
		try 
		{
			if (m_NewValue.m_DomItemNos[i] == -1)
			{
				GetRoss()->InsertDomItem(S, DomNo, m_NewValue.m_DomItemNos[i]);
			}
		}
		catch (...)
		{
		   return false;
		};

	  }

	}
  return true;
};

void CValueDlg::OnOK() 
{
	if (  !SetNewValue() ) return;

	// Это не ошибка. SetNewValue нужно вызывать дважды из-за InsertDomItem, которая не может вставить две новых константы 
	// одновременно
	SetNewValue();
	
	CDialog::OnOK();
}


BOOL CValueDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (   (nID < m_SignatsBtns.size() + 100)
		&& (nID >= 100))
		UpdateData (TRUE);
	
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
