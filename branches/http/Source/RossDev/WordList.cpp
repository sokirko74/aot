// WordList.cpp : implementation file
//
#include "stdafx.h"
#include "WordList.h"
#include "InputBox.h"
#include "../common/utilit.h"
#include "MainFrm.h"
#include "ArticleDoc.h"
#include "FieldList.h"
#include "PocketForm.h"
#include "SchemeRossChoicer.h"
#include "PosChoicer.h"
#include "ImportForm.h"
#include "Translations.h"
#include "NewArticleWizard.h"
#include "NewTextEntry.h"
#include "NewRefArticles.h"
#include "FindByStrDlg.h"
#include "FindWrongRefs.h"
#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWordList

IMPLEMENT_DYNCREATE(CWordList, CSizeFormView)

CWordList::CWordList()
	: CSizeFormView(CWordList::IDD)
{
	//{{AFX_DATA_INIT(CWordList)
	m_UnitsSize = _T("");
	m_LemmaLocator = _T("");
	m_Sorted = SortByLemma;
	//}}AFX_DATA_INIT
}

CWordList::~CWordList()
{
	
}

void CWordList::DoDataExchange(CDataExchange* pDX)
{
	CSizeFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWordList)
	DDX_Control(pDX, IDC_UNITS_SIZE, m_UnitSizeCtrl);
	DDX_Control(pDX, IDC_WORDLIST_DEL, m_DelButton);
	DDX_Control(pDX, IDC_CHANGE_TITLE, m_ChangeTitleButton);
	DDX_Control(pDX, IDC_WORDLIST_ADD, m_AddButton);
	DDX_Control(pDX, IDC_LEMMA_LOCATOR, m_LemmaLocatorCtrl);
	DDX_Control(pDX, IDC_WORDLIST_GRID, m_WordList);
	DDX_Text(pDX, IDC_UNITS_SIZE, m_UnitsSize);
	DDX_Text(pDX, IDC_LEMMA_LOCATOR, m_LemmaLocator);
	//}}AFX_DATA_MAP
}




BEGIN_MESSAGE_MAP(CWordList, CSizeFormView)
	//{{AFX_MSG_MAP(CWordList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_WORDLIST_GRID, OnGetdispinfoWordlistGrid)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_WORDLIST_ADD, OnWordlistAdd)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDR_SAVE_ONLY_WORDLIST,  OnSaveWordlist)
	ON_EN_CHANGE(IDC_LEMMA_LOCATOR, OnChangeLemmaLocator)
	ON_COMMAND(IDR_SELECT_ALL_WORDS,  OnSelectAllWords)
	ON_COMMAND(IDR_INVERT_SELECTION, OnInvertAllWords)
	ON_COMMAND(IDR_DESELECT_ALL_WORDS,  OnDeselectAllWords)
	ON_BN_CLICKED(IDC_WORDLIST_DEL, OnWordlistDel)
	ON_COMMAND(IDR_SET_FILTER, OnSetFilter)
	ON_BN_CLICKED(IDC_SET_SELECTED_BUTTON, OnSetSelectedButton)
	ON_BN_CLICKED(IDC_WORDLIST_ARTICLE_BTN, OnWordlistArticleBtn)
	ON_COMMAND(IDR_SAVE_ROSS_TO_TXT,  OnSaveRossToTxt)
	ON_COMMAND(ID_SEARCH_BY_ARTICLE, OnSearchByArticle)
	ON_COMMAND(IDR_SCHEME_ROSS, OnShowScheme)
	ON_COMMAND(IDR_STATISTIC, OnStatistic)
	ON_BN_CLICKED(IDC_CHANGE_TITLE, OnChangeTitle)
	ON_COMMAND(ID_MENUITEM32786, OnComments)
	ON_COMMAND(ID_MENUITEM32788, OnMenuitem32788)
	ON_COMMAND(ID_VALENCIES, OnValencies)
	ON_COMMAND(ID_SEMFET, OnSemFet)
	ON_COMMAND(ID_STATISTIC_FIELD_VALUE, OnStatisticFieldValue) 
	ON_COMMAND(ID_SORT_BY_LEMMA, OnSortByLemma)
	ON_COMMAND(ID_SORT_BY_DATEANDTIME, OnSortByDateAndTime)
	ON_COMMAND(ID_MENUITEM32784, OnImport)
	ON_COMMAND(IDD_APPENDARTICLE, OnArticleAppend)
	ON_COMMAND(IDR_DEL_ALL_SELECTED, OnDelAllSelected)
	ON_BN_CLICKED(IDC_SET_AUTHOR, OnSetAuthor)
	ON_COMMAND(ID_SELECT_AUTHOR, OnSelectByAuthor)
	ON_COMMAND(IDR_GXI_STATISTICS, OnGXiStatistics)
	ON_NOTIFY(NM_DBLCLK, IDC_WORDLIST_GRID, OnDblclkWordlistGrid)
	ON_COMMAND(ID_MENUITEM32804, OnSelectDownward)
	ON_COMMAND(ID_MENUITEM32805, OnEmptyArticles)
	ON_BN_CLICKED(IDC_READONLY_IMAGE, OnReadonlyImage)
	ON_COMMAND(ID_FIND_BY_STR,OnFindByString)
	ON_COMMAND(ID_FIND_WRONG_REFS,OnFindWrongRefs)
	ON_NOTIFY(NM_CLICK, IDC_WORDLIST_GRID, OnClickWordlistGrid)
	ON_EN_SETFOCUS(IDC_LEMMA_LOCATOR, OnSetfocusLemmaLocator)
	ON_COMMAND(IDD_EXPORT_ALL_DICTS,OnExportAllDicts)
	ON_COMMAND(EMPTY_ALL_DICTS,OnEmptyAllDicts)
	ON_COMMAND(ID_IMPORT_ALL_DICTS,OnImportAllDicts)
	ON_COMMAND(ID_DEL_TEXT_DOMENS, OnDelTextDomains)
	ON_COMMAND(ID_RELOAD, OnReload)
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_ALL_DICT_ENTRIES, OnAllDictEntries)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWordList diagnostics

#ifdef _DEBUG
void CWordList::AssertValid() const
{
	CSizeFormView::AssertValid();
}

void CWordList::Dump(CDumpContext& dc) const
{
	CSizeFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWordList message handlers

#ifndef ListView_SetCheckState
   #define ListView_SetCheckState(hwndLV, i, fCheck) \
      ListView_SetItemState(hwndLV, i, \
      INDEXTOSTATEIMAGEMASK((fCheck)+1), LVIS_STATEIMAGEMASK)
#endif



extern  CRossDevApp theApp;

tm Str2Tm (CString TimeStr)
{
	tm output;
	sscanf (TimeStr, "%i/%i/%i %i:%i:%i", &output.tm_mday,&output.tm_mon,&output.tm_year, &output.tm_hour, &output.tm_min, &output.tm_sec);
	output.tm_mon --;
	return output;
};


string CWordList::GetEntryStr(WORD UnitNo) const
{
	if (!GetDocument()->IsThesRoss())
		return GetRoss()->GetEntryStr(UnitNo);            
	else
	{
		int TerminId = atoi(GetRoss()->GetEntryStr(UnitNo).c_str());
		if (TerminId == 0)
			return  GetRoss()->GetEntryStr(UnitNo);            
		else
		{
			int ThesId = GetDocument()->GetThesId();
			const CThesaurus*  Thes = GetSemBuilder().m_RusStr.m_pData->GetThes(ThesId);
			int TerminNo =  Thes->GetTerminNoByTextEntryId(TerminId);
			if (TerminNo == -1)
				return GetRoss()->GetEntryStr(UnitNo); 
			else
				return Thes->m_Termins[TerminNo].m_TerminStr;
		};
	};

};



void CWordList::OnGetdispinfoWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
	// TODO: Add your control notification handler code here
	LV_ITEM* pItem= &(pDispInfo)->item;
	if (GetUnitsSize() == 0) return;
	char s[10];
	CString S;  
	WORD UnitNo = GetUnitNo(pItem->iItem);





	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{  switch(pItem->iSubItem){
		 case 0: //fill in main text            
			 if (m_Termins.empty())
				 lstrcpy(pItem->pszText, GetRoss()->GetEntryStr(UnitNo).c_str());            
			 else
				 lstrcpy(pItem->pszText, m_Termins[pItem->iItem].m_TerminStr.c_str());            

			 break;
		 case 1: //fill in sub item 1 text            
			 sprintf(s, "%i", GetRoss()->GetUnitMeanNum(UnitNo));            
			 lstrcpy(pItem->pszText, s);            
			 break;
		 case 2: //fill in main text            
			 if (GetDocument()->m_ReadOnly)
				 lstrcpy(pItem->pszText,"");
			 else
			 {
				 const char* coms = GetRoss()->GetCommentsByUnitId(GetRoss()->GetUnits()[UnitNo].m_EntryId)->Comments;
				 lstrcpy(pItem->pszText, coms);            
			 }
			 break;

		 case 3:

			 if (GetDocument()->m_ReadOnly)
				 lstrcpy(pItem->pszText,"");
			 else
				 lstrcpy(pItem->pszText, GetRoss()->GetUnitModifTimeStr(UnitNo).c_str());            
			 break;

		 case 4: 
			 sprintf(s, "%s", GetRoss()->GetUnits()[UnitNo].m_bSelected ? "      +" : "       -");            
			 lstrcpy(pItem->pszText, s);
			 break;
		 case 5: 
			 if (GetDocument()->m_ReadOnly)
				 lstrcpy(pItem->pszText,"");
			 else
			 {
				 lstrcpy(pItem->pszText, GetRoss()->GetUnits()[UnitNo].m_AuthorStr);            
			 }
			 break;
		 case 6:
			 if (GetDocument()->m_ReadOnly)
				 lstrcpy(pItem->pszText,"");
			 else
				 lstrcpy(pItem->pszText, GetRoss()->GetUnitEditor(UnitNo).c_str());            
			 break;


	}
	}

	POSITION pos = m_WordList.GetFirstSelectedItemPosition();
	if (pos != 0)
		UnitNo = m_WordList.GetNextSelectedItem(pos);
	m_UnitsSize.Format ("Cловарный вход %i (из %i)",    UnitNo+1, m_WordList.GetItemCount());
	//UpdateData(FALSE);
}




void CWordList::Update()  
{  
  m_WordList.SetItemCountEx(GetUnitsSize());
  UpdateIndex();
  UpdateCurrentPos();
  m_WordList.UpdateData(FALSE);
  m_WordList.Invalidate();
  
};



void CWordList::BuildIndex()  
{ 
	// для тезаурусных РОССов строится свой индекс в самом начале, который не меняется
	if (GetDocument()->IsThesRoss())
	{
		long Count = GetRoss()->GetUnitsSize();
		m_Termins.clear();
		for (long i=0; i < Count; i++)
			m_Termins.push_back(CRossDevTermin(i, GetEntryStr(i)));
		sort(m_Termins.begin(),m_Termins.end());
		return;
	};

	m_Index.clear();


	WORD UnitsSize = GetUnitsSize();
	for (size_t i = 0; i < UnitsSize; i++)
	{
		CIndex I;
		I.UnitNo = (!IsFiltered()) ? i : GetRoss()->GetSelectedUnitNo(i);

		if (m_Sorted != SortByLemma) // GetUnitModifTimeStr занимает много времени,
			// поэтому отключаем ее в нормальном режиме
		{
			CString TimeStr = GetRoss()->GetUnitModifTimeStr(I.UnitNo).c_str();
			I.modif_tm = Str2Tm(TimeStr);
		}
		m_Index.push_back(I); 
	}


};


const  int DlgWidth = 730;
const  int DlgHeight = 500;


void CWordList::OnInitialUpdate() 
{
	CSizeFormView::OnInitialUpdate(); 

	if (!GetDocument()->m_ReadOnly)
       GetDescendantWindow(  IDC_READONLY_IMAGE)->ShowWindow(SW_HIDE);


    
	
	// установка контекстного меню 
	if (m_Menu.LoadMenu(IDR_POPUP_WORDLIST))
	    SetMenu(&m_Menu);
	else
		CExpc ("Cannot load menu");
    m_Menu.CheckMenuItem (IDR_SET_FILTER,	MF_BYCOMMAND  | MF_UNCHECKED );



	    //установка основного списка
    m_WordList.InsertColumn(1,"Title",LVCFMT_LEFT, 200);
	m_WordList.InsertColumn(2,"Sense",LVCFMT_LEFT, 60);
	m_WordList.InsertColumn(3,"Comm.",LVCFMT_LEFT, 70);
	m_WordList.InsertColumn(4,"Last modif.",LVCFMT_LEFT, 100);
	m_WordList.InsertColumn(5,"Mark",LVCFMT_LEFT, 40);
	m_WordList.InsertColumn(6,"Author",LVCFMT_LEFT, 60);
	m_WordList.InsertColumn(7,"Editor",LVCFMT_LEFT, 60);
	
//	BuildIndex();
    //OnSortByLemma();

    Update();
	if (GetDocument()->IsThesRoss())
	{
		m_ChangeTitleButton.EnableWindow( FALSE);
	};


	m_WordList.SetExtendedStyle(LVS_EX_FULLROWSELECT);  
	if (m_WordList.GetItemCount() > 0) 
      m_WordList.SetItemState( 0,   LVIS_FOCUSED| LVIS_SELECTED , LVIS_FOCUSED|LVIS_SELECTED );

	// установка ширфта 
	CWnd* wnd =  GetDlgItem(IDC_STATIC_LEFT);
	CFont* F = wnd->GetFont(); 
	LOGFONT pLogFont;
	F->GetLogFont( & pLogFont);
	pLogFont.lfHeight = -20;
	pLogFont.lfWeight = FW_BOLD;
	CFont G;
	G.CreateFontIndirect(&pLogFont);
	wnd->SetFont(&G);
	GetDlgItem(IDC_STATIC_RIGHT)->SetFont(&G); 

	// Установка размеров
	int GlobalX = ::GetSystemMetrics(SM_CXSCREEN);
	int GlobalY = ::GetSystemMetrics(SM_CYSCREEN);
	GetParent()->SetWindowPos(NULL, (GlobalX-DlgWidth)/2, (GlobalY-DlgHeight)/2-60, DlgWidth, DlgHeight, SWP_SHOWWINDOW|SWP_NOZORDER);


	
}

void CWordList::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
    if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return;	
	CSizeFormView::OnClose();
}

bool CWordList::SetArticle(WORD UnitNo, CString Value)
{
  CTempArticle A;
  A.m_pRoss =   GetRoss();
  A.ReadFromDictionary(UnitNo, true, false);
  A.SetArticleStr(  (const char*)Value );
  return A.WriteToDictionary();
};


bool CWordList::AddNewRecordToUnits (char* Word, bool bTalk, char* Comments)
{
  try {
  if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return false;
  CPosChoicer PosChoicer; 
  CTranslations Trans;
  CNewTextEntry  TextEntry;
  TextEntry.m_pRoss = GetRoss();
  CNewArticleWizard Sheet ("New entry", GetDocument());
  Sheet.SetWizardMode();
  Sheet.AddPage(&TextEntry);
  Sheet.AddPage(&PosChoicer);
  Sheet.AddPage(&Trans);
  

  if (Sheet.DoModal() == IDOK) 
  {
	WORD UnitNo;
 	if (Sheet.GetPageCount() == 4)
	{
		CString Article = ((CNewRefArticles*)Sheet.GetPage(3))->m_ResultString;
		CString Lemma =  ((CNewRefArticles*)Sheet.GetPage(3))->m_Lemma;
		if (!Article.IsEmpty())
			if (GetRoss()->LocateUnit((const char*)Lemma, 1) != ErrUnitNo)
			{
				AfxMessageBox ("The reference article cannot be created");
			}
			else
			{
              if (Lemma.GetLength() > EntryStrSize) 
			  {
				  CString Q;
				  Q.Format("Cannot add the entry \"%s\", because it is too long", Lemma);
				  AfxMessageBox (Q);
				  return false;
			  };
		  	  UnitNo = GetRoss()->InsertUnit((const char*)Lemma, 1);
			  GetRoss()->GetUnits()[UnitNo].m_bSelected = IsFiltered();
              GetRoss()->SetUnitAuthor(UnitNo,(const char*)((CRossDoc*)GetDocument())->m_Author);
			  GetRoss()->SetUnitEditor(UnitNo,  (const char*)((CRossDoc*)GetDocument())->m_Author);
			  SetArticle(UnitNo, Article);
			  Update();
			};
	};

    if (TextEntry.m_UnitStr.GetLength() > EntryStrSize) 
	 {
	 	  CString Q;
	      Q.Format("Cannot add the entry \"%s\", because it is too long", TextEntry.m_UnitStr);
		  AfxMessageBox (Q);
		  return false;
	 };

    BYTE MeanNum = atoi (TextEntry.m_MeanNum); 
    UnitNo = GetRoss()->InsertUnit((const char*)TextEntry.m_UnitStr, MeanNum);
    GetRoss()->GetUnits()[UnitNo].m_bSelected = IsFiltered ();
    GetRoss()->SetUnitAuthor(UnitNo, (const char*)((CRossDoc*)GetDocument())->m_Author);
	GetRoss()->SetUnitEditor(UnitNo, (const char*)((CRossDoc*)GetDocument())->m_Author);
    Update();
	SetArticle(UnitNo, PosChoicer.m_ResultString + Trans.m_ResultString);
    
	// установка курсора на нужную строку 
    SetCursor (UnitNo);
    GetDocument()->SetModifiedFlag();
  }
  }
  catch (...)
  {
	  AfxMessageBox ("Cannot add the entry");
	  return false;

  };
  return true;
}


void CWordList::OnWordlistAdd() 
{

	if (AddNewRecordToUnits(NULL,true,NULL))
	BuildIndex();
	Update ();
	
	// TODO: Add your control notification handler code here
	
}

void PrintExportHeader (const CWordList& Parent, FILE* fp)
{
	fprintf(fp,"//Export dictionary %s\r\n",Parent.GetRoss()->m_DictName.c_str());
	fprintf (fp,"//Date = %s\r\n", CTime::GetCurrentTime().Format( "%A, %B %d, %Y" )); 
	fprintf (fp,"//Records count = %i\r\n",Parent.GetUnitsSize());
	fprintf (fp,"//Filtered = %s\r\n\r\n",(Parent.IsFiltered () ? "yes" : "no"));
};


void CWordList::OnSaveWordlist() 
{
	// TODO: Add your command handler code here
	try {
		CFileDialog  D (FALSE, "txt","wordlist.txt");
		if (D.DoModal() != IDOK) return;
		FILE* fp = fopen (D.GetPathName(), "wb");
		
		PrintExportHeader(*this, fp);

		for (size_t i = 0;  i < m_WordList.GetItemCount(); i++)
		{
			fprintf (fp,"%s %i\r\n", GetEntryStr(GetUnitNo(i)).c_str(), GetRoss()->GetUnitMeanNum(GetUnitNo(i)));		
		};

		fclose(fp);

	}
	catch (...) {

		AfxMessageBox ("Cannot write file");

	};

	
}

void CWordList::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	   // make sure window is active   
   GetParentFrame()->ActivateFrame();
   CPoint local = point;   
   ScreenToClient(&local);   
   CMenu* pPopup = m_Menu.GetSubMenu(0);
   ASSERT(pPopup != NULL);
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,point.x, point.y,AfxGetMainWnd()); // use main window for cmds      

};

void CWordList::SetSelected(bool Value) 
{
  WORD UnitsSize = GetUnitsSize();
  for (size_t i = 0; i < UnitsSize; i++)
	  GetRoss()->GetUnits()[GetUnitNo(i)].m_bSelected = Value;

};


void CWordList::OnSelectAllWords() 
{
	SetSelected (true);
	m_WordList.Invalidate();
};

void CWordList::OnDeselectAllWords() 
{
	SetSelected (false);
	m_WordList.Invalidate();
};

void CWordList::OnInvertAllWords() 
{
	WORD UnitsSize = GetUnitsSize();
	for (size_t i = 0; i < UnitsSize; i++)
	{
		bool R = !GetRoss()->GetUnits()[GetUnitNo(i)].m_bSelected ;
		GetRoss()->GetUnits()[GetUnitNo(i)].m_bSelected = R;
	};

	m_WordList.Invalidate();
};



void CWordList::SetCursor(int i) 
{
	LVITEM lv;
	memset (&lv, 0, sizeof(LVITEM));
	lv.mask  = LVIF_STATE;
	lv.iItem = i;
	lv.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lv.state = LVIS_SELECTED | LVIS_FOCUSED;
	m_WordList.SetItemState (i, &lv);
	m_WordList.EnsureVisible( i, FALSE );

	m_WordList.Invalidate();

}

struct LessByUnitNo {
	bool operator() (const CIndex &C1, const WORD& UnitNo)  const
	{
		return C1.UnitNo < UnitNo;
	};
	bool operator() (const WORD& UnitNo, const CIndex &C2)  const
	{
		return UnitNo < C2.UnitNo ;
	};
	bool operator() (const CIndex &C1, const CIndex &C2)  const
	{
		return C1.UnitNo < C2.UnitNo;
	};
};
		

void CWordList::OnChangeLemmaLocator() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSizeFormViewView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if (!GetDocument()->IsThesRoss())
	{
		WORD i = GetRoss()->UnitsLowerBound((const char*)m_LemmaLocator);
		WORD k = lower_bound (m_Index.begin(), m_Index.end(), i, LessByUnitNo()) - m_Index.begin();
		SetCursor (k);
	}
	else
	{
		CRossDevTermin T (0, (const char*)m_LemmaLocator);
		vector<CRossDevTermin>::iterator It = lower_bound(m_Termins.begin(),m_Termins.end(),T);
		SetCursor (It - m_Termins.begin());

	};
	// установка курсора на нужную строку 

};


void CWordList::OnWordlistDel() 
{
	// TODO: Add your control notification handler code here
    if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return;

	WORD UnitNo;
	if (!GetSelectedUnitNo (UnitNo)) return;
	CString S;
	S.Format("Delte entry %s%i ?",GetEntryStr(UnitNo).c_str(), GetRoss()->GetUnitMeanNum(UnitNo));
   	if (::MessageBox(0, S, "Message Box", MB_OKCANCEL) != IDOK)
	return;
	GetRoss()->DelUnit(GetRoss()->GetUnits().begin() + UnitNo);
	BuildIndex();
	Update();
	
    GetDocument()->SetModifiedFlag();
}

void CWordList::OnSetFilter()
{
  if (IsFiltered ())
    m_Menu.CheckMenuItem (IDR_SET_FILTER,	MF_BYCOMMAND  | MF_UNCHECKED );
  else
	m_Menu.CheckMenuItem (IDR_SET_FILTER,	MF_BYCOMMAND  | MF_CHECKED );
  BuildIndex();
  Update();
};

void CWordList::OnSetSelectedButton() 
{
	// TODO: Add your control notification handler code here
    WORD UnitNo;
	if (!GetSelectedUnitNo (UnitNo)) return;
	bool b = GetRoss()->GetUnits()[UnitNo].m_bSelected;
	GetRoss()->GetUnits()[UnitNo].m_bSelected = !b;
	m_WordList.Invalidate();
	CDocument* D = GetDocument();
    D->SetModifiedFlag();
}


void CWordList::OnWordlistArticleBtn() 
{
	// TODO: Add your control notification handler code here
	// получения ссылки на текущуь словарную статью
	try 
	{
      WORD UnitNo;
	  if (!GetSelectedUnitNo (UnitNo)) return;
	  GlobalOpenArticle (GetDocument(), UnitNo);
	}
	catch  (...)
	{
		AfxMessageBox("Unexpected error!");
	};
}


void CWordList::SaveRossToTxt(CString FileName) const 
{
	try{	

		FILE * fp = fopen (FileName,"wb");

		PrintExportHeader(*this, fp);

		CTempArticle A;
		A.m_pRoss = GetRoss();

		for (size_t i = 0;  i < GetUnitsSize(); i++)
		{ 
			fprintf (fp,"============\r\n");
			string  s = GetRoss()->GetUnitTextHeader(GetUnitNo(i));
			fprintf (fp,"%s", s.c_str());

			try 
			{
				A.ReadFromDictionary(GetUnitNo(i), false, true);
				fprintf (fp,"%s", A.GetArticleStr().c_str());
			}
			catch (...)
			{
				fprintf (fp,"Error! Cannot get the article\r\n");
			}

		};

		fclose (fp);
	}
	catch (...) {
		AfxMessageBox ("Cannot write file");

	};

};

void CWordList::OnSaveRossToTxt()
{
	CWaitCursor C;
	CFileDialog D(FALSE, "txt", "ross.txt");
	if (D.DoModal() != IDOK) return;
	SaveRossToTxt(D.GetPathName());
};

CDocument* CWordList::FindArticle (WORD UnitNo)
{
	CDocTemplate* templ  = GetRossArticleTemplate();
	POSITION pos = templ->GetFirstDocPosition();
	CDocument* D;

	do {
		  D = templ->GetNextDoc(pos);
		  if (!D ) break;
		  if ( UnitNo == ((CArticleDoc*)D)->m_UnitNo)
			  break;
		}
	while (D != 0);

	return D;
};




void CWordList::OnSearchByArticle()
{
	CWaitCursor C;
    WORD UnitNo;
	if (!GetSelectedUnitNo (UnitNo)) return;
	vector<CRossPocketItem> UnitNos;

	if (GetRoss()->IsEmptyArticle(UnitNo))
	{
		AfxMessageBox (CString("No search by empty article!"));
		return ;
	};

    CTempArticle A1;
	A1.m_pRoss = GetRoss();
	A1.ReadFromDictionary(UnitNo, false, true);
	CTempArticle A2;
	A2.m_pRoss = GetRoss();

    for (size_t i = 0;  i < GetUnitsSize(); i++)
 	if (GetUnitNo(i) != UnitNo)  
	{ 
		vector<TCortege> L2;
        A2.ReadFromDictionary(GetUnitNo(i), false, true);
		if (A1.IsPartOf(&A2, true))
			UnitNos.push_back(CRossPocketItem(GetUnitNo(i), GetDocument()));
	};
	C.Restore();

	CString S;
	S.Format ("%s%i",GetEntryStr(UnitNo).c_str(), GetRoss()->GetUnitMeanNum(UnitNo));
	OpenPocket(UnitNos, CString(" dictionary entry ") + S);
};


void CWordList::OnShowScheme()
{
  if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return;
  
  SchemeRossChoicer dlg (GetDocument());
  dlg.DoModal();
  GetDocument()->BuildBasicDomItems();
  
};


struct CStatis { 
	BYTE  DomNo;
	LONG  ItemNo;
	int Freq;
	bool operator==(const CStatis& X) const
			{return     ItemNo == X.ItemNo; }

    bool operator<(const CStatis& X) const
	  { return  (ItemNo < X.ItemNo); }
};

void CWordList::OnStatistic()
{
	CWaitCursor C;
	CRossDoc* pRossDoc = GetDocument();
	vector<CStatis> V;
	char s[200];
	s[0] = 0;
	if ( !InputBox("Введите название домена (* - все константные домены):", s, 200) )
		return;
	CString Q(s);
	Q.TrimLeft();
	Q.TrimRight();
	if (Q[0] == '*')
		for (size_t i = 0; i < pRossDoc->m_BasicDomItems.size(); i++)
		{
			CStatis S;
			S.ItemNo = GetRoss()->GetItemNoByItemStr(pRossDoc->m_BasicDomItems[i].ItemStr,  pRossDoc->m_BasicDomItems[i].DomNo);
			S.Freq = 0;
			if (S.ItemNo != -1)
				V.push_back(S);
		}
	else
	{
		BYTE DomNo = GetRoss()->GetDomenNoByDomStr((const char*)Q);
		if (DomNo == ErrUChar) 
		{
			AfxMessageBox ("No such domen");
			return;
		};
		for (size_t i=0; i<GetRoss()->GetDomItemsSize(); i++)
		{
			if (GetRoss()->GetDomItemDomNo(i) == DomNo) 
			{
				CStatis S;
				S.ItemNo = i;
				S.DomNo = DomNo;
				S.Freq = 0;
				V.push_back(S);
			};
		};

	};

	s[0] = 0; 
	char caption[200];
	sprintf (caption, "Введите число элементов кортежа, которые надо сравнивать (1..%i)", GetRoss()->m_MaxNumDom);
	if (!InputBox(caption, s, 200))
		return;

	size_t NumDom = atoi(s);
	if (NumDom > MaxNumDom) 
	{
		AfxMessageBox ("The value is too great");
		return;
	};


	sort (V.begin(),V.end());

	for (size_t i = 0;  i<GetRoss()->_GetCortegesSize(); i++)
	{
		for (size_t k=0; k < NumDom; k++)
			if (k != -1)
			{

				CStatis I;
				I.ItemNo = GetCortege(GetRoss(), i).m_DomItemNos[k];
				vector<CStatis>::iterator It = lower_bound (V.begin(),V.end(), I);
				if (    (It != V.end())
					&& (I == *It))
					It->Freq++;
			};
	};


	CString S = " Домен                Константа              Частота \r\n_______________________________________________________\r\n" ;
	for (size_t k = 0; k < V.size(); k++)
	{
		CString Q;
		BYTE DomNo = GetRoss()->GetDomItemDomNo(V[k].ItemNo);
		CString DomStr  = GetRoss()->m_Domens[DomNo].DomStr;
		Q.Format ("%-16s %-5s %-20s %-5s %i \r\n",DomStr, "|", (const char*)GetRoss()->GetDomItemStr(V[k].ItemNo), "|", V[k].Freq);
		//if (!GetRoss()->GetDomainIsDelim(DomNo))
		S += Q;
	};

	GlobalOpenReport (S, "Частота констант");
}

void CWordList::OnChangeTitle() 
{
	// TODO: Add your control notification handler code here
  if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return;

  WORD UnitNo;
  if (!GetSelectedUnitNo (UnitNo)) return;
  CString UnitStr = GetRoss()->GetEntryStr(UnitNo).c_str();
  int i = GetRoss()->LocateUnit ((const char*)UnitStr,2);
  if (i != ErrUnitNo)
  {
	   AfxMessageBox ("You cannot change entry name, if this entry has many senses");  
  	   return;
  };

  char s[EntryStrSize];
  strcpy  (s, UnitStr);
  if (!InputBox("Edit entry name:", s, EntryStrSize))
  return;
  if (!strcmp (s, UnitStr)) return;

  i = GetRoss()->LocateUnit (s,1);
  if (i != ErrUnitNo)
  {
	   AfxMessageBox ("This entry name already exists");  
	   return;
	};

  GetRoss()->SetUnitStr(UnitNo, s);
  GetDocument()->SetModifiedFlag();
  BuildIndex();
  Update();
}

void CWordList::OnComments()
{
  WORD UnitNo;
  if (!GetSelectedUnitNo (UnitNo)) return;
  if (GetDocument()->m_ReadOnly) return;
			 
  const TUnitComment* pComms = GetRoss()->GetCommentsByUnitId(GetRoss()->GetUnits()[UnitNo].m_EntryId);
  CString  C = pComms->Comments;
  char s[100];
  strcpy  (s, C);
  if (!InputBox("Комментарий:", s, 100,"РОСС",this))
  return;
  if (!strcmp (s, C)) return;
  GetRoss()->SetUnitCommentStr(UnitNo, s);
  GetDocument()->SetModifiedFlag();
  Update();

};


// иерархия отношений
void CWordList::OnMenuitem32788() 
{
  // TODO: Add your command handler code here
  GlobalOpenHierarchy (GetDocument(), SemRel);	
 
}



void CWordList::BuildVals (vector<Valency>& Vals, WORD UnitNo)
{
	 Vals.clear();
	 if (GetRoss()->IsEmptyArticle(UnitNo)) return;
	 for (size_t k = GetRoss()->GetUnitStartPos(UnitNo); k <= GetRoss()->GetUnitEndPos(UnitNo); k++)
		 if (GetCortege(GetRoss(), k).m_FieldNo == GetDocument()->GetRossHolder()->ValFieldNo)
		 {
			 Valency V;
			 V.ValNo = GetCortege(GetRoss(), k).m_DomItemNos[0];
			 V.A_C = GetCortege(GetRoss(), k).m_DomItemNos[1] != GetDocument()->GetRossHolder()->SelfLabelNo;
			 Vals.push_back(V);
		 };
};






// построить дерево вложения валентных структур 
void CWordList::OnValencies()
{
try {
  CWaitCursor C;
  CHierarchyHolder D (GetDocument()->GetRossHolder());
  D.m_Type = SemRel;
  D.MySerialize(false, true);

  TreeNode Tree; 	
  if  (    (GetDocument()->GetRossHolder()->ValFieldNo == ErrUChar)
	    || (GetDocument()->GetRossHolder()->ActDomNo == ErrUChar) 
		|| (GetDocument()->GetRossHolder()->SelfLabelNo == -1)
	  )
  {
	  AfxMessageBox ("This operation is not applicable!");
      return;
  };
  
  for (size_t i = 0;  i < GetUnitsSize(); i++)
  {
	 vector<Valency> Vals;

	 BuildVals (Vals, GetUnitNo(i));

     if (Vals.size() == 0) continue;

	 sort (Vals.begin(), Vals.end());

	 vector<TreeNode>::iterator It = find (Tree.SubItems.begin(), Tree.SubItems.end(), Vals);
	 if (It == Tree.SubItems.end())
	 {
		 TreeNode N;
		 N.Vals = Vals;
		 N.UnitNos.push_back(GetUnitNo(i));
		 Tree.SubItems.push_back(N);
	 }
	 else
		 It->UnitNos.push_back(GetUnitNo(i));

  };

  sort (Tree.SubItems.begin(), Tree.SubItems.end());
  size_t k;

  TreeNode NormTree;
  for (k = 0; k < Tree.SubItems.size(); k++)
  {
   TreeNode N; 
   N.Vals =  Tree.SubItems[k].Vals;
   TreeNode::Normalize(D, N.Vals);
   vector<TreeNode>::iterator It = find (NormTree.SubItems.begin(), NormTree.SubItems.end(), N.Vals);
   if (It == NormTree.SubItems.end())
   { 
	 NormTree.SubItems.push_back (N);
	 It =  NormTree.SubItems.begin() + NormTree.SubItems.size() - 1;
   };
   It->SubItems.push_back(Tree.SubItems[k]);
  };

  sort (NormTree.SubItems.begin(), NormTree.SubItems.end());
  
  CString S = "_______________________________________________________\r\n" ;

  for (k = 0; k < NormTree.SubItems.size(); k++)
   {
	  CString Record = NormTree.SubItems[k].GetVals (GetRoss());
	  Record += "\r\n";

      for (size_t j = 0; j < NormTree.SubItems[k].SubItems.size(); j++)
	  {
       CString Line;
	   TreeNode& N = NormTree.SubItems[k].SubItems[j];
	   Line.Format("   %i %s // ",N.UnitNos.size(), N.GetVals(GetRoss()));

	   for (int i=0; (i < N.UnitNos.size()) && (i < 8); i++)
	   { 
		 CString Q;
	     Q.Format("%s%i ", GetRoss()->GetEntryStr(N.UnitNos[i]).c_str(), GetRoss()->GetUnitMeanNum(N.UnitNos[i]));
		 Line += Q;
	   };

	   if (N.UnitNos.size() > 8) 
		   Line += "...";

	   Line += "\r\n";
	   Record += Line;
	   
	  };
	  S += Record;
  };

  GlobalOpenReport (S, "Валентные структуры");
}
 catch (...)
 {
	  AfxMessageBox ("an error occured");
 };
};

void CWordList::OnSemFet() 
{
  // TODO: Add your command handler code here
  GlobalOpenHierarchy (GetDocument(), SemFet);	
 
}


  

struct CFieldValue {
 	TCortege cortege;
	BYTE FieldNo;
    int      freq; 
	BYTE	 m_MaxNumDom;

	bool operator==(const CFieldValue& X) const 
	{  
		TCortege C = cortege;
		TCortege XX = X.cortege;
		return		(FieldNo == X.FieldNo)  
				&&	C.HasEqualItems(XX, m_MaxNumDom); 
	}

	bool operator<	(const CFieldValue& X) const 
	{
		return  (FieldNo < X.FieldNo)  && !cortege.HasEqualItems(X.cortege, m_MaxNumDom); 
	}

};

void CWordList::OnStatisticFieldValue()
{
	vector<CFieldValue> V;
	char s[200];
	s[0] = 0;
	if ( !InputBox("Введите название поля (* - все поля):", s, 200) ) return;
	BYTE FieldNo = ErrUChar;
	if (s[0] != '*')
	{
		FieldNo = GetRoss()->GetFieldNoByFieldStr(s);
		if (FieldNo == ErrUChar)
		{
			AfxMessageBox ("Cannot find this field");
			return;
		};
	};




	CWaitCursor C;

	for (size_t i = 0;  i<GetRoss()->_GetCortegesSize(); i++)
	{
		if  (    (FieldNo != ErrUChar) 
			&& (GetCortege(GetRoss(), i).m_FieldNo != FieldNo)
			)
			continue;


		CFieldValue Value;
		Value.cortege = GetCortege(GetRoss(), i);
		Value.freq = 1;
		Value.FieldNo = GetCortege(GetRoss(), i).m_FieldNo;
		Value.m_MaxNumDom = GetRoss()->m_MaxNumDom;
		vector<CFieldValue>::iterator It = find (V.begin(), V.end(), Value);
		if (It == V.end()) 
			V.push_back(Value);
		else
			It->freq++;
	};

	sort(V.begin(), V.end());
	CString S = "_______________________________________________________\r\n" ;

	for (int i = 0; i < V.size(); i++)
	{
		BYTE FieldNo = V[i].FieldNo;

		CString Q;
		Q.Format("%-16s %-5s %-30s %-5s %-16i\r\n",
			(const char*)GetRoss()->Fields[V[i].FieldNo].FieldStr, "|", 
			WriteToString (GetRoss(), (char*)GetRoss()->Fields[FieldNo].m_Signats[V[i].cortege.GetSignatNo()].sFrmt, V[i].cortege).c_str(), "|", V[i].freq);
		S += Q;
	};

	GlobalOpenReport (S, "Заполнение словарных полей");


};



void CWordList::OnSortByLemma()
{
	if (GetDocument()->IsThesRoss()) return;
	m_Menu.CheckMenuItem (ID_SORT_BY_LEMMA,	MF_BYCOMMAND  | MF_CHECKED );
    m_Menu.CheckMenuItem (ID_SORT_BY_DATEANDTIME,	MF_BYCOMMAND  | MF_UNCHECKED );
	if (m_Sorted == SortByLemma) return;
    m_Sorted = SortByLemma;
	m_LemmaLocatorCtrl.EnableWindow(TRUE); 

   
	Update();
};


void CWordList::OnSortByDateAndTime()
{
	if (GetDocument()->IsThesRoss()) return;
	m_Menu.CheckMenuItem (ID_SORT_BY_LEMMA,	MF_BYCOMMAND  | MF_UNCHECKED );
	m_Menu.CheckMenuItem (ID_SORT_BY_DATEANDTIME,	MF_BYCOMMAND  | MF_CHECKED );
	if (m_Sorted == SortByDateAndTime) return;
	m_Sorted = SortByDateAndTime;
	m_LemmaLocatorCtrl.EnableWindow(FALSE);
	Update();
	
};





int CompareIndex( const void *arg1, const void *arg2 )
{
	CIndex* C1 = (CIndex*) arg1;
	CIndex* C2 = (CIndex*) arg2;
	if (*C1 < * C2)
		return -1;
	else
		if (*C2 < *C1)
			return 1;
        else
		    return 0;

};

void CWordList::UpdateIndex()
{
	CWaitCursor C;

	if (m_Sorted == SortByLemma)
	  BuildIndex();
	else
	{
	  BuildIndex();
   	  qsort(&(*(m_Index.begin())), m_Index.size(), sizeof(CIndex), CompareIndex); 
	};         

};



	
void CWordList::OnImport()
{
  CImportForm dlg (GetDocument());
  dlg.DoModal();
  Update();
};


void UpdateWordAndFreq (vector <CWordAndFreq>& Unfound, const string& Word)  
{
	  CWordAndFreq W;
	  W.m_Word = Word;
	  W.m_Freq = 1;
	  vector<CWordAndFreq>::iterator It = lower_bound(Unfound.begin(), Unfound.end(), W);
	  size_t i = It - Unfound.begin();
	  if (   (It == Unfound.end()) 
		  || !(*It == W)
		 )
	  {
		Unfound.insert(It, W);
	  }
	  else
		Unfound[i].m_Freq++;
};

void CWordList::OnArticleAppend()
{
	try 
	{
		WORD MainUnitNo;
		if (!GetSelectedUnitNo (MainUnitNo)) return;

		CTempArticle A1;
		A1.m_pRoss = GetRoss();
		if (MainUnitNo != ErrUnitNo)
			A1.ReadFromDictionary(MainUnitNo, false, false);

		CTempArticle A2;
		A2.m_pRoss = GetRoss();

		CString S;
		if (::MessageBox(this->m_hWnd, "The environment add the current entry to all entries, if there is no intersection by fields. Procees?", "Confirmation",MB_YESNO) == IDNO) return;

		for (size_t i = 0;  i < GetUnitsSize(); i++)
		{ 
			WORD UnitNo = GetUnitNo(i);
			if (MainUnitNo == UnitNo) continue;
			A2.ReadFromDictionary(UnitNo, false, false);

			A2.AddArticle(&A1);
			if (!A2.WriteToDictionary())
			{
				AfxMessageBox ("Some error has occured");
				break;
			}
		};
	}
	catch (...)
	{
		AfxMessageBox ("Some error has occured");
	};
	//GlobalOpenReport (S, "Добавление подстатей");
};


struct TUnit{
	char UnitStr[200];
	BYTE MeanNum;
};
void CWordList::OnDelAllSelected()
{
 try {
  CString Q; 
  Q.Format ("You are going to delete all entries (%i entries). Proceed?", GetUnitsSize());
  if (::MessageBox(this->m_hWnd, Q, "Confirmation",MB_YESNO) == IDNO) return;
  CWaitCursor C;
  vector <TUnit> UnitNos;

  for (size_t  i = 0; i < GetUnitsSize(); i++)
  {
	  size_t u  = GetUnitNo(i);
	  TUnit U;
	  strcpy (U.UnitStr, GetRoss()->GetEntryStr(u).c_str());
	  U.MeanNum = GetRoss()->GetUnitMeanNum(u);
	  UnitNos.push_back(U);

  };

  for (int i =0;  i< UnitNos.size(); i++)
  { 
	WORD UnitNo = GetRoss()->LocateUnit(UnitNos[i].UnitStr, UnitNos[i].MeanNum);
	GetRoss()->DelUnit(GetRoss()->GetUnits().begin() + UnitNo);
	
  };

  BuildIndex();
  Update();
  GetDocument()->SetModifiedFlag();
 }
 catch (...) 
 {
	 AfxMessageBox ("An exception occured!");

 };
};

void CWordList::OnSetAuthor()
{
  if  (GetDocument()->m_ReadOnly) return;
  CString Q; 
  Q.Format ("You are going to change the author of all entries(%i entries). Proceed?", GetUnitsSize());
  if (::MessageBox(this->m_hWnd, Q, "Confirmation",MB_YESNO) == IDNO) return;
  char s[200];
  s[0] = 0;
  if ( !InputBox("Введите имя автора:", s, 15) ) return;
  CWaitCursor C;
  vector <TUnit> UnitNos;

  for (size_t  i = 0; i < GetUnitsSize(); i++)
  {
	  size_t u  = GetUnitNo(i);
	  GetRoss()->SetUnitAuthor(u,s);

  };


  Update();
  GetDocument()->SetModifiedFlag();
};

void CWordList::OnSelectByAuthor()
{
	char s[200];
	s[0] = 0;
	if ( !InputBox("Введите имя автора:", s, 15) ) return;
	CWaitCursor C;

	for (size_t  i = 0; i < GetUnitsSize(); i++)
	{
		size_t u  = GetUnitNo(i);
		CString Author = GetRoss()->GetUnits()[u].m_AuthorStr;
		if (Author == CString(s))
			GetRoss()->GetUnits()[u].m_bSelected = true;

	};


	Update();
	GetDocument()->SetModifiedFlag();
};

struct GxiStatistic
{
	CString		m_SemRelName;
	TCortege	m_GXi;
	int			m_num;
	BYTE		m_MaxNumDom;
	vector<long> Units;
	GxiStatistic (CString SemRelName,	TCortege GXi, long UnitNo, BYTE MaxNumDom)
	{
		m_SemRelName = SemRelName;
	    m_GXi = GXi;
		m_num = 1;
		m_MaxNumDom = MaxNumDom;
		Units.push_back(UnitNo);
	};
	bool operator<(const GxiStatistic& GX) const 
	{
		if( m_SemRelName < GX.m_SemRelName)
			return true;

		if( m_SemRelName > GX.m_SemRelName)
			return false;

		if( m_SemRelName == GX.m_SemRelName)
			if( m_num > GX.m_num )
				return true;

		return false;
	}
	bool operator==(const GxiStatistic& GX) const 
	{
		return (m_SemRelName == GX.m_SemRelName) && m_GXi.HasEqualItems(GX.m_GXi, m_MaxNumDom);
	};
	
};

void CWordList::OnGXiStatistics()
{
	vector<GxiStatistic> GxiStatisticVector;
	for (size_t  i = 0; i < GetUnitsSize(); i++)
	{
		size_t UnitNo  = GetUnitNo(i);
		if (GetRoss()->IsEmptyArticle(UnitNo)) continue;

		vector<CValency> Vals;
		for (size_t k = GetRoss()->GetUnitStartPos(UnitNo); k<= GetRoss()->GetUnitEndPos(UnitNo); k++)
			if ( GetRoss()->GetCortegeFieldNo(k) ==  GetDocument()->GetRossHolder()->ValFieldNo )
				Vals.push_back(CValency (GetCortege(GetRoss(),k),GetDocument()->GetRossHolder()->MainWordVarNo, GetDocument()->GetRossHolder()));


		for (int k = GetRoss()->GetUnitStartPos(UnitNo); k<= GetRoss()->GetUnitEndPos(UnitNo); k++)
		{
			if (GetRoss()->GetCortegeFieldNo(k) != GetDocument()->GetRossHolder()->GramFetFieldNo) break;

			int ValencyNo = GetRoss()->GetCortegeLeafId(k) - 1;
			if ( (ValencyNo < 0) ||  (ValencyNo >= Vals.size()) ) break;
			GxiStatistic GX (Vals[ValencyNo].m_RelationStr.c_str(), GetCortege(GetRoss(), k), UnitNo, GetRoss()->m_MaxNumDom);
			vector<GxiStatistic>::iterator It = find (GxiStatisticVector.begin(), GxiStatisticVector.end(), GX);
			if (It == GxiStatisticVector.end())
				GxiStatisticVector.push_back(GX);
			else
			{
				It->m_num++;
				It->Units.push_back(UnitNo);
			};

		};


	};


	sort(GxiStatisticVector.begin(), GxiStatisticVector.end());
	CString Protocol;
	for(int j = 0 ; j < GxiStatisticVector.size() ; j++ )
	{
		CString S;
		BYTE FieldNo = GxiStatisticVector[j].m_GXi.m_FieldNo;
		CString CortegeStr  = WriteToString (GetRoss(), (char*)GetRoss()->Fields[FieldNo].m_Signats[GxiStatisticVector[j].m_GXi.GetSignatNo()].sFrmt, GxiStatisticVector[j].m_GXi).c_str();

		S.Format ("%-16s %-5s %-20s %-5s %i ",GxiStatisticVector[j].m_SemRelName, "|", CortegeStr, "|", GxiStatisticVector[j].m_num);
		for(int k = 0 ; k <  GxiStatisticVector[j].Units.size() ; k++ )
		{
			S += GetEntryStr(GxiStatisticVector[j].Units[k]).c_str();
			S +="  ";
		}
		S += "\r\n";
		Protocol += S;
	}

	GlobalOpenReport (Protocol, "Frequence of GFi");
};	


void CWordList::OnSelectDownward()
{


	POSITION pos = m_WordList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{ ::MessageBox(0, "No selection found", "Message Box", MB_OK);
	return;};

	for (size_t i =m_WordList.GetNextSelectedItem(pos); i <m_WordList.GetItemCount(); i++)  
	{
		WORD UnitNo = GetUnitNo(i);
		GetRoss()->GetUnits()[UnitNo].m_bSelected = true;
	};
	Update();

};


void CWordList::OnEmptyArticles()
{
	try {
	  CString Q; 
	  Q.Format ("You are going to empty all entries (%i entries). Proceed?", GetUnitsSize());
	  if (::MessageBox(this->m_hWnd, Q, "Confirmation",MB_YESNO) == IDNO) return;
	  CWaitCursor C;
	  vector <TUnit> UnitNos;

	  for (size_t  i = 0; i < GetUnitsSize(); i++)
	  {
		  size_t u  = GetUnitNo(i);
		  TUnit U;
		  strcpy (U.UnitStr, GetRoss()->GetEntryStr(u).c_str());
		  U.MeanNum = GetRoss()->GetUnitMeanNum(u);
		  UnitNos.push_back(U);

	  };

	  for (int i =0;  i< UnitNos.size(); i++)
	  { 
		WORD UnitNo = GetRoss()->LocateUnit(UnitNos[i].UnitStr, UnitNos[i].MeanNum);
		GetRoss()->ClearUnit(UnitNo);
		
	  };

	  BuildIndex();
	  Update();
	  GetDocument()->SetModifiedFlag();
	}
	catch (...)
	{
	 AfxMessageBox ("An exception occured!");
	};
};

void CWordList::OnReload()
{
	if (GetDocument()->IsModified())
	{
		if (::MessageBox(0, "You are going to lose all changes, which you have made", "Confirmation", MB_OKCANCEL) != IDOK)
			return;
	};

	if (!GetDocument()->m_ReadOnly)
		GetDocument()->RemoveLock();

	CFile F(GetDocument()->GetPathName()  , CFile::modeRead);
	CArchive ar(&F, CArchive::load);
	GetDocument()->Serialize(ar);
	BuildIndex();
	Update();
	GetDocument()->SetModifiedFlag(FALSE);
	GetDescendantWindow(  IDC_READONLY_IMAGE)->ShowWindow(GetDocument()->m_ReadOnly ? SW_SHOW : SW_HIDE);

};

void CWordList::OnReadonlyImage() 
{
	// TODO: Add your control notification handler code here
	CString FileName = GetDocument()->GetLockFileName().c_str();
	if (access((const char*)FileName, 0) != -1)
	{
	    char s [1000];
		CString Mess;
        FILE* fp =  fopen (FileName, "r");
		if (!fp) return;
		while (fgets(s, 1000, fp))
			Mess += s;
		fclose(fp);
		::MessageBox (0, Mess, "Lock Information", MB_OK);
		
	};
}
void CWordList::OnFindByString()
{
  CFindByStrDlg FindByStrDlg(this, GetDocument());
  FindByStrDlg.DoModal();


};

void CWordList::OnFindWrongRefs()
{
  CFindWrongRefs FindByStrDlg(this, GetDocument());
  FindByStrDlg.DoModal();


};

void CWordList::UpdateCurrentPos()
{
  POSITION pos = m_WordList.GetFirstSelectedItemPosition();
  WORD UnitNo;
  if (pos != 0)
	{
	  UnitNo = m_WordList.GetNextSelectedItem(pos);
      m_UnitsSize.Format ("Entry No %i (из %i)",    UnitNo+1, m_WordList.GetItemCount());
      UpdateData(FALSE);
	};

};





void CWordList::OnClickWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateCurrentPos();
	m_UnitSizeCtrl.Invalidate();
	
	*pResult = 0;
}

void CWordList::OnDblclkWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
    OnWordlistArticleBtn();
	*pResult = 0;
}

void CWordList::OnKeydownWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{


	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateCurrentPos();
	m_UnitSizeCtrl.Invalidate();
	
	*pResult = 0;
}



LRESULT CWordList::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	/*
	Когда фокус стоит в списке или локаторе клавига ENTER открывает статью.
	*/
	if ( (message == 273) && (wParam ==1))
	{
	   CWnd* FocusWnd = GetFocus();
	   if  (    (FocusWnd == &m_WordList) 
		     || (FocusWnd == &m_LemmaLocatorCtrl) 
		   )
			  OnWordlistArticleBtn();
	}
	

	
	return CSizeFormView::DefWindowProc(message, wParam, lParam);
}

//void SetKeyboardLanguage (USHORT  LangId)
//{
//	HKL FirstCurrLangId = 0;
//	for (long i=0; i < 5; i++)
//	{
//		char s[100];
//		GetKeyboardLayoutName (s);
//		int CurrLangId;
//		sscanf (s,"%i",&CurrLangId); 
//
//		CurrLangId &= 0xFF;
//
//		if ( CurrLangId == LangId) break;
//		if (i == 0)
//		 FirstCurrLangId = ActivateKeyboardLayout((HKL)HKL_NEXT, 0);
//		else
//		  ActivateKeyboardLayout((HKL)HKL_NEXT, 0);
//	};
//
//	if (i == 5)
//		if (FirstCurrLangId > 0)
//	{
//		ActivateKeyboardLayout((HKL)FirstCurrLangId, 0);
//	};
//};


void CWordList::OnSetfocusLemmaLocator() 
{
	// TODO: Add your control notification handler code here
 //if (GetDocument()->m_LanguageId == Russian)
//	 SetKeyboardLanguage (LANG_RUSSIAN);
	   
 //if  (GetDocument()->m_LanguageId == English)
//	 SetKeyboardLanguage (LANG_ENGLISH);
}

BOOL FGetDirectory(LPTSTR szDir, HWND hwnd) 
{ 
	BOOL  fRet; 
	TCHAR  szPath[MAX_PATH]; 
	LPITEMIDLIST pidl; 
	LPITEMIDLIST pidlRoot; 
	LPMALLOC lpMalloc;  
	BROWSEINFO bi = { hwnd, NULL, szPath, "Choose directory", BIF_RETURNONLYFSDIRS, NULL, 0L, 0};  
	if (0 != SHGetSpecialFolderLocation(HWND_DESKTOP, CSIDL_DRIVES, &pidlRoot)) 
	return FALSE; 
	if (NULL == pidlRoot) 
	return FALSE;  
	bi.pidlRoot = pidlRoot; 
	pidl = SHBrowseForFolder(&bi);  

	if (NULL != pidl) 
		fRet = SHGetPathFromIDList(pidl, szDir); 
	else 
		fRet = FALSE;  // Get the shell's allocator to free PIDLs 

	if (!SHGetMalloc(&lpMalloc) && (NULL != lpMalloc)) 
	{ 
		if (NULL != pidlRoot) { lpMalloc->Free(pidlRoot); }  

		if (NULL != pidl) { lpMalloc->Free(pidl); }  

		lpMalloc->Release(); 
	}  
	return fRet; 
} 

void CWordList::OnExportAllDicts()
{
	try{
		char Folder[1000];
		FGetDirectory(Folder, this->m_hWnd);

		CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
		POSITION pos = pRossDocTemplate->GetFirstDocPosition();
		int RossNo = 0;
		while( pos )
		{
				CDocument* pDoc= pRossDocTemplate->GetNextDoc(pos);	
				POSITION pos1 = pDoc->GetFirstViewPosition();
				if (pos1 == 0) return;
				CWordList* pView = (CWordList*)pDoc->GetNextView(pos1);
				CString FileName;
				FileName.Format("%s\\ross%i.txt", Folder, RossNo++);
				pView->SaveRossToTxt(FileName);
		};
	}
	catch (...)
	{
	 AfxMessageBox ("An exception occured!");
	};
};


bool UnlockAllDicts()
{
try{
		CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
		POSITION pos = pRossDocTemplate->GetFirstDocPosition();
		int RossNo = 0;
		while( pos )
		{
				CDocument* pDoc= pRossDocTemplate->GetNextDoc(pos);	
				POSITION pos1 = pDoc->GetFirstViewPosition();
				if (pos1 == 0) return false;
				
				CWordList* pView = (CWordList*)pDoc->GetNextView(pos1);
				int i = 0;
				while (pView->GetDocument()->m_ReadOnly)
				{
					CString S;
					S.Format ("The dictionary %s is locked. Delete lock file !",pView->GetDocument()->GetRossHolder()->m_DictName.c_str());
					if (i == 1)
						AfxMessageBox (S);
					
					pView->OnReload();
					if (i == 1) 
					{
						S.Format ("The dictionary %s is locked. Exit from the procedure!",pView->GetDocument()->GetRossHolder()->m_DictName.c_str());
						return false;
					};
					i++;
				};
				CString FileName;
		};
	}
	catch (...)
	{
	 AfxMessageBox ("An exception occured!");
	 return false;
	};
	return true;
	
}
void CWordList::OnEmptyAllDicts()
{
	if (!UnlockAllDicts()) return;
	try{
		CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
		POSITION pos = pRossDocTemplate->GetFirstDocPosition();
		int RossNo = 0;
		while( pos )
		{
				CDocument* pDoc= pRossDocTemplate->GetNextDoc(pos);	
				POSITION pos1 = pDoc->GetFirstViewPosition();
				if (pos1 == 0) return;
				
				CWordList* pView = (CWordList*)pDoc->GetNextView(pos1);
				pView->OnDelAllSelected();
		};
	}
	catch (...)
	{
	 AfxMessageBox ("An exception occured!");
	};

};

void CWordList::OnImportAllDicts()
{
	if (!UnlockAllDicts()) return;
	char Folder[1000];
	FGetDirectory(Folder, this->m_hWnd);


	try{
		CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
		POSITION pos = pRossDocTemplate->GetFirstDocPosition();
		int RossNo = 0;
		while( pos )
		{
				CDocument* pDoc= pRossDocTemplate->GetNextDoc(pos);	
				POSITION pos1 = pDoc->GetFirstViewPosition();
				if (pos1 == 0) return;
				
				CWordList* pView = (CWordList*)pDoc->GetNextView(pos1);
				CImportForm dlg ((CRossDoc*)pDoc);
				dlg.m_FileName.Format("%s\\ross%i.txt", Folder, RossNo++);
				dlg.m_SimulatingMode = FALSE;
				if (dlg.DoModal() != IDOK) return;
				pView->Update();
		};
	}
	catch (...)
	{
	 AfxMessageBox ("An exception occured!");
	};

 
};

void CWordList::OnDelTextDomains() 
{
	// TODO: Add your command handler code here
	if (!UnlockAllDicts()) return;
	try{
		CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
		POSITION pos = pRossDocTemplate->GetFirstDocPosition();
		int RossNo = 0;
		while( pos )
		{
				CRossDoc* pDoc= (CRossDoc*)pRossDocTemplate->GetNextDoc(pos);
				if (pDoc->GetRoss()->_GetCortegesSize() > 0 )
				{
					 AfxMessageBox ("Delete all dictionary entries before!");
					 return;
				};
				pDoc->DelTextDomains();
		};
	}
	catch (...)
	{
	 AfxMessageBox ("An exception occured!");
	};
}


struct CDictInterp {
	string					m_UnitStr;
	BYTE					m_MeanNum;
	const CWordList*		m_Owner;
	bool operator <  (const CDictInterp& X) const 
	{
		if (m_UnitStr !=  X.m_UnitStr)
			return m_UnitStr <  X.m_UnitStr;

		if (m_Owner == X.m_Owner)
			return m_MeanNum < X.m_MeanNum;
		else
			return m_Owner < X.m_Owner;
	};
};

void AddEntries (const CWordList* WordList, vector<CDictInterp>& AllEntries) 
{
	if (!WordList->GetDocument()->IsThesRoss())
	{
	  for (size_t  i = 0; i < WordList->GetUnitsSize(); i++)
	  {
		  size_t u  = WordList->GetUnitNo(i);
		  CDictInterp I;
		  I.m_UnitStr =  WordList->GetRoss()->GetEntryStr(i);            
		  I.m_MeanNum = WordList->GetRoss()->GetUnitMeanNum(u);
		  I.m_Owner = WordList;
		  AllEntries.push_back(I);
	  }
	}
	else
	{
		int ThesId = WordList->GetDocument()->GetThesId();
		const CThesaurus*  Thes = GetSemBuilder().m_RusStr.m_pData->GetThes(ThesId);
		for (int i=0; i <Thes->m_Termins.size(); i++)
		{
		    CDictInterp I;
			I.m_UnitStr =  Thes->m_Termins[i].m_TerminStr;            
			I.m_MeanNum = 1;
			I.m_Owner = WordList;
		    AllEntries.push_back(I);
		};
	};
};
void CWordList::OnAllDictEntries() 
{
	try{
	   	bool bAllEntries = ::MessageBox(0, "Export only entries with many senses?", "Confirmation", MB_YESNO) != IDYES;

		CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
		POSITION pos = pRossDocTemplate->GetFirstDocPosition();
		int RossNo = 0;
		vector<CDictInterp> AllEntries;
		while( pos )
		{
				const CRossDoc* pDoc= (const CRossDoc*) pRossDocTemplate->GetNextDoc(pos);	
				POSITION pos1 = pDoc->GetFirstViewPosition();
				if (pos1 == 0) return;
				const CWordList* pView = (CWordList*)pDoc->GetNextView(pos1);
				AddEntries(pView, AllEntries);
	
		};
	    
		sort (AllEntries.begin(), AllEntries.end());
		if (!bAllEntries)
		{
			for (long i=0; i < AllEntries.size()-1; i++)
				if (AllEntries[i].m_UnitStr !=  AllEntries[i+1].m_UnitStr)
				{
					AllEntries.erase(AllEntries.begin() + i);
					i--;
				}
				else
				{
					for (; i < AllEntries.size()-1; i++)
						if (AllEntries[i].m_UnitStr !=  AllEntries[i+1].m_UnitStr)
							break;
				};
				if (AllEntries.size() == 1)
					AllEntries.clear();
				if (AllEntries.size() > 1)
					if (AllEntries.back().m_UnitStr != AllEntries[AllEntries.size() - 2].m_UnitStr)
						AllEntries.pop_back();

		};

		string Result;
		for (long i=0; i < AllEntries.size(); i++)
		{
		   Result += Format ("%-40s %s %i\n",AllEntries[i].m_UnitStr.c_str(), 
			   AllEntries[i].m_Owner->GetDocument()->GetRossHolder()->m_DictName.c_str(),
			   AllEntries[i].m_MeanNum);
			   
		};
		GlobalOpenReport (Result.c_str(), Format("Number of Entries: %i", AllEntries.size()).c_str() );
	}
	catch (...)
	{
	 AfxMessageBox ("An exception occured!");
	};
	// TODO: Add your command handler code here
	
}
