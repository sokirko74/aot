// PocketForm.cpp : implementation file
//

#include "stdafx.h"
#include "PocketForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPocketForm dialog


IMPLEMENT_DYNCREATE(CPocketForm, CFormView)

CPocketForm::CPocketForm()
	: CFormView(CPocketForm::IDD)

{
	//{{AFX_DATA_INIT(CPocketForm)
	m_UnitsSize = _T("");
	//}}AFX_DATA_INIT
	
}


void CPocketForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPocketForm)
	DDX_Control(pDX, IDC_LIST1, m_WordList);
	DDX_Text(pDX, IDC_UNITS_SIZE, m_UnitsSize);
	//}}AFX_DATA_MAP
}

#ifdef _DEBUG
void CPocketForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CPocketForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG




BEGIN_MESSAGE_MAP(CPocketForm, CFormView)
	//{{AFX_MSG_MAP(CPocketForm)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, OnGetdispinfoList1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkWordlistGrid)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//IMPLEMENT_DYNCREATE(CPocketForm, CFormView)

/////////////////////////////////////////////////////////////////////////////
// CPocketForm message handlers

void CPocketForm::OnGetdispinfoList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	LV_ITEM* pItem= &(pDispInfo)->item;
	char s[100];
	WORD UnitNo = m_PocketItems[pItem->iItem].m_UnitNo;
	CRossDoc* RossDoc = m_PocketItems[pItem->iItem].m_pRossDoc;
	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{  switch(pItem->iSubItem){
		 case 0: //fill in main text            
			 lstrcpy(pItem->pszText, RossDoc->GetRoss()->GetEntryStr(UnitNo).c_str());            
			 break;
		 case 1: //fill in sub item 1 text            
			 sprintf(s, "%i", RossDoc->GetRoss()->GetUnitMeanNum(UnitNo));            
			 lstrcpy(pItem->pszText, s);            
			 break;
		 case 2: 
			 lstrcpy(pItem->pszText, RossDoc->GetRossHolder()->m_DictName.c_str());            
			 break;

	};
	};
	*pResult = 0;
}

void CPocketForm::OnButton1() 
{
	// TODO: Add your control notification handler code here
    WORD UnitNo;
	if (!GetSelectedUnitNo (UnitNo)) return;
	GlobalOpenArticle (m_PocketItems[UnitNo].m_pRossDoc, m_PocketItems[UnitNo].m_UnitNo);
}

BOOL CPocketForm::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CFormView::PreCreateWindow(cs);
}

int CPocketForm::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
    
	
	// TODO: Add your specialized creation code here
	
	return 0;
}


int OpenPocket(   const vector<CRossPocketItem>& UnitNos,
			  CString    Title) 
{

	CDocTemplate* tmpl = GetRossPocketTemplate();;

	CDocument* pDocument = tmpl->CreateNewDocument();

	if (pDocument == NULL)
	{
		TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return false;
	}

    ASSERT_VALID(pDocument);

	pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
	CFrameWnd* pFrame = tmpl->CreateNewFrame(pDocument, NULL);
	pDocument->m_bAutoDelete = TRUE;
	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete pDocument;       // explicit delete on error
		return FALSE;
	};

	ASSERT_VALID(pFrame);

	pDocument->SetPathName(CString("An entry subset by  ")+Title);
	

	// open an existing document
	tmpl->InitialUpdateFrame(pFrame, pDocument, TRUE);

	POSITION pos =  pDocument->GetFirstViewPosition();
	CPocketForm* V = (CPocketForm*)(pDocument->GetNextView(pos));
	V->m_PocketItems = UnitNos;
    V->m_WordList.InsertColumn(1,"Словарный вход", LVCFMT_LEFT, 200);
	V->m_WordList.InsertColumn(2,"Номер значения", LVCFMT_LEFT, 60);
	V->m_WordList.InsertColumn(2,"Название словаря", LVCFMT_LEFT, 60);
	V->m_WordList.SetItemCountEx(UnitNos.size());
	V->m_UnitsSize.Format ("Число словарных входов : %i",    V->m_WordList.GetItemCount());
    V->UpdateData(FALSE);
    V->m_WordList.UpdateData(FALSE);
    V->m_WordList.Invalidate();	
	V->m_Title = Title;

	// установка размеров
	V->GetParent()->SetWindowPos(NULL, 0,0, 535 , 500, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOMOVE);
	return true;
};

// записать
void CPocketForm::OnButton2() 
{
	// TODO: Add your control notification handler code here
    CFileDialog D(FALSE, "txt", m_Title);
	if (D.DoModal() != IDOK) return;
	FILE * fp = fopen (D.GetPathName(),"wb");
	for (size_t i=0; i < m_PocketItems.size(); i++)
	{
		CRossDoc* RossDoc = m_PocketItems[i].m_pRossDoc;
		fprintf (fp, "%s%i\r\n", RossDoc->GetRoss()->GetEntryStr(m_PocketItems[i].m_UnitNo).c_str(), RossDoc->GetRoss()->GetUnitMeanNum(m_PocketItems[i].m_UnitNo));
	}
	fclose (fp);
}


// отобразить в пометы 
void CPocketForm::OnButton3() 
{
	// TODO: Add your control notification handler code here
	for (size_t i=0; i < m_PocketItems.size(); i++)
	{
		CRossDoc* RossDoc = m_PocketItems[i].m_pRossDoc;
		RossDoc->GetRoss()->GetUnits()[m_PocketItems[i].m_UnitNo].m_bSelected =  true;
	};
	
}


void CPocketForm::OnDblclkWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
    OnButton1();
	*pResult = 0;
};