// PocketForm.cpp : implementation file
//

#include "StdAfx.h"
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
	uint16_t UnitNo = m_PocketItems[pItem->iItem].m_UnitNo;
	CRossDoc* RossDoc = m_PocketItems[pItem->iItem].m_pRossDoc;
	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{  switch(pItem->iSubItem){
		 case 0: //fill in main text    
		 {
			 auto s16 = _U16(RossDoc->GetRoss()->GetEntryStr(UnitNo));
			 lstrcpy(pItem->pszText, s16);
			 break;
		 }
		 case 1: //fill in sub item 1 text            
		 {
			 CString s1;
			 s1.Format(_T("%i"), RossDoc->GetRoss()->GetUnitMeanNum(UnitNo));
			 lstrcpy(pItem->pszText, s1);
			 break;
		 }
		 case 2: 
		 {			 
			 auto sd = RossDoc->GetRossHolder()->GetDictName();
			 lstrcpy(pItem->pszText, _U16(sd));            
			 break;
		 }

	};
	};
	*pResult = 0;
}

void CPocketForm::OnButton1() 
{
	// TODO: Add your control notification handler code here
    uint16_t UnitNo;
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


int OpenPocket(   const std::vector<CRossPocketItem>& UnitNos,
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
    V->m_WordList.InsertColumn(1, _T("Dictionary entry"), LVCFMT_LEFT, 200);
	V->m_WordList.InsertColumn(2, _T("Sense ID"), LVCFMT_LEFT, 60);
	V->m_WordList.InsertColumn(2, _T("Dictionary name"), LVCFMT_LEFT, 60);
	V->m_WordList.SetItemCountEx(UnitNos.size());
	V->m_UnitsSize.Format (_T("Number dictionary entries: %i"),    V->m_WordList.GetItemCount());
    V->UpdateData(FALSE);
    V->m_WordList.UpdateData(FALSE);
    V->m_WordList.Invalidate();	
	V->m_Title = Title;

	V->GetParent()->SetWindowPos(NULL, 0,0, 535 , 500, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOMOVE);
	return true;
};

// записать
void CPocketForm::OnButton2() 
{
    CFileDialog D(FALSE, _T("txt"), m_Title);
	if (D.DoModal() != IDOK) return;
	FILE * fp = fopen (_U8(D.GetPathName()).c_str(), "wb");
	for (auto& p: m_PocketItems)
	{
		auto ross = p.m_pRossDoc->GetRoss();
		fprintf (fp, "%s%i\r\n", 
			ross->GetEntryStr(p.m_UnitNo).c_str(), 
			ross->GetUnitMeanNum(p.m_UnitNo));
	}
	fclose (fp);
}


// отобразить в пометы 
void CPocketForm::OnButton3() 
{
	for (auto& item: m_PocketItems)
	{
		CRossDoc* RossDoc = item.m_pRossDoc;
		RossDoc->GetRoss()->GetEntries()[item.m_UnitNo].m_bSelected =  true;
	};
	
}


void CPocketForm::OnDblclkWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
    OnButton1();
	*pResult = 0;
};