// WordList.cpp : implementation file
//
#include "StdAfx.h"
#include "WordList.h"
#include "InputBox.h"
#include "morph_dict/common/utilit.h"
#include "MainFrm.h"
#include "ArticleDoc.h"
#include "PocketForm.h"
#include "PosChoicer.h"
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
	ON_COMMAND(IDR_SAVE_ONLY_WORDLIST, OnSaveWordlist)
	ON_EN_CHANGE(IDC_LEMMA_LOCATOR, OnChangeLemmaLocator)
	ON_COMMAND(IDR_SELECT_ALL_WORDS, OnSelectAllWords)
	ON_COMMAND(IDR_INVERT_SELECTION, OnInvertAllWords)
	ON_COMMAND(IDR_DESELECT_ALL_WORDS, OnDeselectAllWords)
	ON_BN_CLICKED(IDC_WORDLIST_DEL, OnWordlistDel)
	ON_COMMAND(IDR_SET_FILTER, OnSetFilter)
	ON_BN_CLICKED(IDC_SET_SELECTED_BUTTON, OnSetSelectedButton)
	ON_BN_CLICKED(IDC_WORDLIST_ARTICLE_BTN, OnWordlistArticleBtn)
	ON_COMMAND(IDR_SAVE_ROSS_TO_TXT, OnSaveRossToTxt)
	ON_COMMAND(ID_SEARCH_BY_ARTICLE, OnSearchByArticle)
	ON_BN_CLICKED(IDC_CHANGE_TITLE, OnChangeTitle)
	ON_COMMAND(ID_MENUITEM32786, OnComments)
	ON_COMMAND(ID_MENUITEM32788, OnMenuitem32788)
	ON_COMMAND(ID_SEMFET, OnSemFet)
	ON_COMMAND(ID_STATISTIC_FIELD_VALUE, OnStatisticFieldValue)
	ON_COMMAND(ID_SORT_BY_LEMMA, OnSortByLemma)
	ON_COMMAND(ID_SORT_BY_DATEANDTIME, OnSortByDateAndTime)
	ON_COMMAND(IDD_APPENDARTICLE, OnArticleAppend)
	ON_COMMAND(IDR_DEL_ALL_SELECTED, OnDelAllSelected)
	ON_BN_CLICKED(IDC_SET_AUTHOR, OnSetAuthor)
	ON_COMMAND(ID_SELECT_AUTHOR, OnSelectByAuthor)
	ON_NOTIFY(NM_DBLCLK, IDC_WORDLIST_GRID, OnDblclkWordlistGrid)
	ON_COMMAND(ID_MENUITEM32804, OnSelectDownward)
	ON_COMMAND(ID_MENUITEM32805, OnEmptyArticles)
	ON_COMMAND(ID_FIND_BY_STR, OnFindByString)
	ON_COMMAND(ID_FIND_WRONG_REFS, OnFindWrongRefs)
	ON_NOTIFY(NM_CLICK, IDC_WORDLIST_GRID, OnClickWordlistGrid)
	ON_COMMAND(ID_RELOAD, OnReload)
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
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

tm Str2Tm(CString TimeStr)
{
	tm output;
	sscanf(_U8(TimeStr).c_str(), "%i/%i/%i %i:%i:%i", &output.tm_mday, &output.tm_mon, &output.tm_year, &output.tm_hour, &output.tm_min, &output.tm_sec);
	output.tm_mon--;
	return output;
};

uint16_t CWordList::GetUnitNo(uint16_t i) const
{
	if (m_Termins.empty())
		return m_Index[i].UnitNo;
	else
		return m_Termins[i].m_UnitNo;
}

uint16_t CWordList::GetUnitsSize() const
{
	return IsFiltered() ? GetRoss()->GetSelectedUnitsSize() : GetRoss()->GetUnitsSize();
};

bool CWordList::GetSelectedUnitNo(uint16_t& UnitNo) const
{
	POSITION pos = m_WordList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		::MessageBox(0, _T("No selection in the list"), _T("Message Box"), MB_OK);
		return false;
	};

	UnitNo = GetUnitNo(m_WordList.GetNextSelectedItem(pos));

	return true;

};


std::string CWordList::GetEntryStrUtf8(uint16_t UnitNo) const
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
			const CThesaurus* Thes = GetSemBuilder().m_RusStr.m_pData->GetThes(ThesId);
			int TerminNo = Thes->GetTerminNoByTextEntryId(TerminId);
			if (TerminNo == -1)
				return GetRoss()->GetEntryStr(UnitNo);
			else
				return Thes->m_Termins[TerminNo].m_TerminStr;
		};
	};

};
CString CWordList::GetEntryStrUtf16(uint16_t UnitNo) const {
	return _U16(GetEntryStrUtf8(UnitNo));
}



void CWordList::OnGetdispinfoWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
	LV_ITEM* pItem = &(pDispInfo)->item;
	if (GetUnitsSize() == 0) return;
	CString S;
	uint16_t UnitNo = GetUnitNo(pItem->iItem);
	CString entry_str;
	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		switch (pItem->iSubItem) {
		case 0: //fill in main text
			if (m_Termins.empty()) {
				entry_str = _U16(GetRoss()->GetEntryStr(UnitNo));
			}
			else {
				entry_str = m_Termins[pItem->iItem].m_TerminStr;
			}
			lstrcpy(pItem->pszText, entry_str);

			break;
		case 1: //fill in sub item 1 text            
			S.Format(_T("%i"), GetRoss()->GetUnitMeanNum(UnitNo));
			lstrcpy(pItem->pszText, S);
			break;
		case 2: //fill in main text            
			if (GetDocument()->m_ReadOnly)
				lstrcpy(pItem->pszText, _T(""));
			else
			{
				std::string coms = GetRoss()->GetCommentsByUnitId(GetRoss()->GetEntries()[UnitNo].m_EntryId)->Comments;
				lstrcpy(pItem->pszText, _U16(coms));
			}
			break;

		case 3:

			if (GetDocument()->m_ReadOnly)
				lstrcpy(pItem->pszText, _T(""));
			else
				lstrcpy(pItem->pszText, _U16(GetRoss()->GetUnitModifTimeStr(UnitNo)));
			break;

		case 4:
			S = GetRoss()->GetEntries()[UnitNo].m_bSelected ? _T("      +") : _T("       -");
			lstrcpy(pItem->pszText, S);
			break;
		case 5:
			if (GetDocument()->m_ReadOnly)
				lstrcpy(pItem->pszText, _T(""));
			else
			{
				lstrcpy(pItem->pszText, _U16(GetRoss()->GetEntries()[UnitNo].GetAuthorStr()));
			}
			break;
		case 6:
			if (GetDocument()->m_ReadOnly)
				lstrcpy(pItem->pszText, _T(""));
			else
				lstrcpy(pItem->pszText, _U16(GetRoss()->GetUnitEditor(UnitNo)));
			break;


		}
	}

	POSITION pos = m_WordList.GetFirstSelectedItemPosition();
	if (pos != 0)
		UnitNo = m_WordList.GetNextSelectedItem(pos);
	m_UnitsSize.Format(_T("Dictionary entry %i (out of %i)"),
		UnitNo + 1, m_WordList.GetItemCount());
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
		for (long i = 0; i < Count; i++)
			m_Termins.push_back(CRossDevTermin(i, GetEntryStrUtf16(i)));
		sort(m_Termins.begin(), m_Termins.end());
		return;
	};

	m_Index.clear();


	uint16_t UnitsSize = GetUnitsSize();
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
		GetDescendantWindow(IDC_READONLY_IMAGE)->ShowWindow(SW_HIDE);




	// установка контекстного меню 
	if (m_Menu.LoadMenu(IDR_POPUP_WORDLIST))
		SetMenu(&m_Menu);
	else
		throw CExpc("Cannot load menu");
	m_Menu.CheckMenuItem(IDR_SET_FILTER, MF_BYCOMMAND | MF_UNCHECKED);

	//установка основного списка
	m_WordList.InsertColumn(1, _T("Title"), LVCFMT_LEFT, 200);
	m_WordList.InsertColumn(2, _T("Sense"), LVCFMT_LEFT, 60);
	m_WordList.InsertColumn(3, _T("Comm."), LVCFMT_LEFT, 70);
	m_WordList.InsertColumn(4, _T("Last modif."), LVCFMT_LEFT, 100);
	m_WordList.InsertColumn(5, _T("Mark"), LVCFMT_LEFT, 40);
	m_WordList.InsertColumn(6, _T("Author"), LVCFMT_LEFT, 60);
	m_WordList.InsertColumn(7, _T("Editor"), LVCFMT_LEFT, 60);

	Update();
	if (GetDocument()->IsThesRoss())
	{
		m_ChangeTitleButton.EnableWindow(FALSE);
	};


	m_WordList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	if (m_WordList.GetItemCount() > 0)
		m_WordList.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

	// установка ширфта 
	CWnd* wnd = GetDlgItem(IDC_STATIC_LEFT);
	CFont* F = wnd->GetFont();
	LOGFONT pLogFont;
	F->GetLogFont(&pLogFont);
	pLogFont.lfHeight = -20;
	pLogFont.lfWeight = FW_BOLD;
	CFont G;
	G.CreateFontIndirect(&pLogFont);
	wnd->SetFont(&G);
	GetDlgItem(IDC_STATIC_RIGHT)->SetFont(&G);

	// Установка размеров
	int GlobalX = ::GetSystemMetrics(SM_CXSCREEN);
	int GlobalY = ::GetSystemMetrics(SM_CYSCREEN);
	GetParent()->SetWindowPos(NULL, (GlobalX - DlgWidth) / 2, (GlobalY - DlgHeight) / 2 - 60, DlgWidth, DlgHeight, SWP_SHOWWINDOW | SWP_NOZORDER);



}

void CWordList::OnClose()
{
	if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return;
	CSizeFormView::OnClose();
}

bool CWordList::SetArticle(uint16_t UnitNo, CString Value)
{
	try {
		CTempArticle A(GetRoss());
		A.ReadFromDictionary(UnitNo, true, false);
		A.ReadFromUtf8String(_U8(Value).c_str());
		A.WriteToDictionary();
	}
	catch (article_parse_error& a) {
		AfxMessageBox(_U16(a.what()));
		return false;
	}
	return true;
};


bool CWordList::AddNewRecordToUnits(char* Word, bool bTalk, char* Comments)
{
	try {
		if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return false;
		CPosChoicer PosChoicer;
		CTranslations Trans;
		CNewTextEntry  TextEntry;
		TextEntry.m_pRoss = GetRoss();
		CNewArticleWizard Sheet(_T("New entry"), GetDocument());
		Sheet.SetWizardMode();
		Sheet.AddPage(&TextEntry);
		Sheet.AddPage(&PosChoicer);
		Sheet.AddPage(&Trans);


		if (Sheet.DoModal() == IDOK)
		{
			uint16_t UnitNo;
			if (Sheet.GetPageCount() == 4)
			{
				CString Article = ((CNewRefArticles*)Sheet.GetPage(3))->m_ResultString;
				CString Lemma = ((CNewRefArticles*)Sheet.GetPage(3))->m_Lemma;
				if (!Article.IsEmpty())
					if (GetRoss()->LocateUnit(_U8(Lemma).c_str(), 1) != ErrUnitNo)
					{
						AfxMessageBox(_T("The reference article cannot be created"));
					}
					else
					{
						UnitNo = GetRoss()->InsertUnit(_U8(Lemma).c_str(), 1);
						GetRoss()->GetEntries()[UnitNo].m_bSelected = IsFiltered();
						GetRoss()->SetUnitAuthor(UnitNo, _U8(((CRossDoc*)GetDocument())->m_Author).c_str() );
						GetRoss()->SetUnitEditor(UnitNo, _U8(((CRossDoc*)GetDocument())->m_Author).c_str() );
						SetArticle(UnitNo, Article);
						Update();
					};
			};

			BYTE MeanNum = _ttoi(TextEntry.m_MeanNum);
			UnitNo = GetRoss()->InsertUnit(_U8(TextEntry.m_UnitStr).c_str(), MeanNum);
			GetRoss()->GetEntries()[UnitNo].m_bSelected = IsFiltered();
			GetRoss()->SetUnitAuthor(UnitNo, _U8(((CRossDoc*)GetDocument())->m_Author).c_str());
			GetRoss()->SetUnitEditor(UnitNo, _U8(((CRossDoc*)GetDocument())->m_Author).c_str());
			Update();
			SetArticle(UnitNo, PosChoicer.m_ResultString + Trans.m_ResultString);

			// установка курсора на нужную строку 
			SetCursor(UnitNo);
			GetDocument()->SetModifiedFlag();
		}
	}
	catch (CExpc& e)
	{
		AfxMessageBox(_U16(e.what()));
	}
	catch (...)
	{
		AfxMessageBox(_T("Cannot add the entry"));
		return false;

	};
	return true;
}


void CWordList::OnWordlistAdd()
{

	if (AddNewRecordToUnits(NULL, true, NULL))
		BuildIndex();
	Update();

	// TODO: Add your control notification handler code here

}

void PrintExportHeader(const CWordList& Parent, FILE* fp)
{
	fprintf(fp, "//Export dictionary %s\r\n", Parent.GetRoss()->GetDictName().c_str());
	fprintf(fp, "//Date = %s\r\n", _U8(CTime::GetCurrentTime().Format("%A, %B %d, %Y")).c_str());
	fprintf(fp, "//Records count = %i\r\n", Parent.GetUnitsSize());
	fprintf(fp, "//Filtered = %s\r\n\r\n", (Parent.IsFiltered() ? "yes" : "no"));
};


void CWordList::OnSaveWordlist()
{
	try {
		CFileDialog  D(FALSE, _T("txt"), _T("wordlist.txt"));
		if (D.DoModal() != IDOK) return;
		FILE* fp = fopen(_U8(D.GetPathName()).c_str(), "wb");

		PrintExportHeader(*this, fp);

		for (size_t i = 0; i < m_WordList.GetItemCount(); i++)
		{
			fprintf(fp, "%s %i\r\n", GetEntryStrUtf8(GetUnitNo(i)).c_str(), GetRoss()->GetUnitMeanNum(GetUnitNo(i)));
		};

		fclose(fp);

	}
	catch (...) {

		AfxMessageBox(_T("Cannot write file"));

	};


}

void CWordList::OnContextMenu(CWnd* pWnd, CPoint point)
{
	  // make sure window is active   
	GetParentFrame()->ActivateFrame();
	CPoint local = point;
	ScreenToClient(&local);
	CMenu* pPopup = m_Menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd()); // use main window for cmds      

};

void CWordList::SetSelected(bool Value)
{
	uint16_t UnitsSize = GetUnitsSize();
	for (size_t i = 0; i < UnitsSize; i++)
		GetRoss()->GetEntries()[GetUnitNo(i)].m_bSelected = Value;

};


void CWordList::OnSelectAllWords()
{
	SetSelected(true);
	m_WordList.Invalidate();
};

void CWordList::OnDeselectAllWords()
{
	SetSelected(false);
	m_WordList.Invalidate();
};

void CWordList::OnInvertAllWords()
{
	uint16_t UnitsSize = GetUnitsSize();
	for (size_t i = 0; i < UnitsSize; i++)
	{
		bool R = !GetRoss()->GetEntries()[GetUnitNo(i)].m_bSelected;
		GetRoss()->GetEntries()[GetUnitNo(i)].m_bSelected = R;
	};

	m_WordList.Invalidate();
};



void CWordList::SetCursor(int i)
{
	LVITEM lv;
	memset(&lv, 0, sizeof(LVITEM));
	lv.mask = LVIF_STATE;
	lv.iItem = i;
	lv.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lv.state = LVIS_SELECTED | LVIS_FOCUSED;
	m_WordList.SetItemState(i, &lv);
	m_WordList.EnsureVisible(i, FALSE);

	m_WordList.Invalidate();

}

struct LessByUnitNo {
	bool operator() (const CIndex& C1, const uint16_t& UnitNo)  const
	{
		return C1.UnitNo < UnitNo;
	};
	bool operator() (const uint16_t& UnitNo, const CIndex& C2)  const
	{
		return UnitNo < C2.UnitNo;
	};
	bool operator() (const CIndex& C1, const CIndex& C2)  const
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
		uint16_t i = GetRoss()->UnitsLowerBound(_U8(m_LemmaLocator).c_str());
		uint16_t k = lower_bound(m_Index.begin(), m_Index.end(), i, LessByUnitNo()) - m_Index.begin();
		SetCursor(k);
	}
	else
	{
		CRossDevTermin T(0, m_LemmaLocator);
		auto it = lower_bound(m_Termins.begin(), m_Termins.end(), T);
		SetCursor(it - m_Termins.begin());

	};
	// установка курсора на нужную строку 

};


void CWordList::OnWordlistDel()
{
	// TODO: Add your control notification handler code here
	if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return;

	uint16_t UnitNo;
	if (!GetSelectedUnitNo(UnitNo)) return;
	CString S;
	S.Format(_T("Delete entry %s%i ?"), 
		GetEntryStrUtf16(UnitNo), GetRoss()->GetUnitMeanNum(UnitNo));
	if (::MessageBox(0, S, _T("Message Box"), MB_OKCANCEL) != IDOK)
		return;
	GetRoss()->DelUnit(UnitNo);
	BuildIndex();
	Update();

	GetDocument()->SetModifiedFlag();
}

void CWordList::OnSetFilter()
{
	if (IsFiltered())
		m_Menu.CheckMenuItem(IDR_SET_FILTER, MF_BYCOMMAND | MF_UNCHECKED);
	else
		m_Menu.CheckMenuItem(IDR_SET_FILTER, MF_BYCOMMAND | MF_CHECKED);
	BuildIndex();
	Update();
};

void CWordList::OnSetSelectedButton()
{
	// TODO: Add your control notification handler code here
	uint16_t UnitNo;
	if (!GetSelectedUnitNo(UnitNo)) return;
	bool b = GetRoss()->GetEntries()[UnitNo].m_bSelected;
	GetRoss()->GetEntries()[UnitNo].m_bSelected = !b;
	m_WordList.Invalidate();
	CDocument* D = GetDocument();
	D->SetModifiedFlag();
}


void CWordList::OnWordlistArticleBtn()
{
	// получения ссылки на текущуь словарную статью
	try
	{
		uint16_t UnitNo;
		if (!GetSelectedUnitNo(UnitNo)) return;
		GlobalOpenArticle(GetDocument(), UnitNo);
	}
	catch (...)
	{
		AfxMessageBox(_T("Unexpected error!"));
	};
}


void CWordList::SaveRossToTxt(CString FileName) const
{
	try {

		FILE* fp = fopen(_U8(FileName).c_str(), "wb");

		PrintExportHeader(*this, fp);

		CTempArticle A(GetRoss());

		for (size_t i = 0; i < GetUnitsSize(); i++)
		{
			fprintf(fp, "============\r\n");
			std::string  s = GetRoss()->GetUnitTextHeader(GetUnitNo(i));
			fprintf(fp, "%s", s.c_str());

			try
			{
				A.ReadFromDictionary(GetUnitNo(i), false, true);
				fprintf(fp, "%s", A.GetArticleStrUtf8().c_str());
			}
			catch (...)
			{
				fprintf(fp, "Error! Cannot get the article\r\n");
			}

		};

		fclose(fp);
	}
	catch (...) {
		AfxMessageBox(_T("Cannot write file"));

	};

};

void CWordList::OnSaveRossToTxt()
{
	CWaitCursor C;
	CFileDialog D(FALSE, _T("txt"), _T("ross.txt"));
	if (D.DoModal() != IDOK) return;
	SaveRossToTxt(D.GetPathName());
};

CDocument* CWordList::FindArticle(uint16_t UnitNo)
{
	CDocTemplate* templ = GetRossArticleTemplate();
	POSITION pos = templ->GetFirstDocPosition();
	CDocument* D;

	do {
		D = templ->GetNextDoc(pos);
		if (!D) break;
		if (UnitNo == ((CArticleDoc*)D)->m_UnitNo)
			break;
	} while (D != 0);

	return D;
};




void CWordList::OnSearchByArticle()
{
	CWaitCursor C;
	uint16_t UnitNo;
	if (!GetSelectedUnitNo(UnitNo)) return;
	std::vector<CRossPocketItem> UnitNos;

	if (GetRoss()->IsEmptyArticle(UnitNo))
	{
		AfxMessageBox(CString("No search by empty article!"));
		return;
	};

	CTempArticle A1(GetRoss());
	A1.ReadFromDictionary(UnitNo, false, true);

	CTempArticle A2(GetRoss());

	for (size_t i = 0; i < GetUnitsSize(); i++)
		if (GetUnitNo(i) != UnitNo)
		{
			std::vector<TCortege> L2;
			A2.ReadFromDictionary(GetUnitNo(i), false, true);
			if (A1.IsPartOf(&A2, true))
				UnitNos.push_back(CRossPocketItem(GetUnitNo(i), GetDocument()));
		};
	C.Restore();

	CString S;
	S.Format(_T("%s%i"), GetEntryStrUtf16(UnitNo), GetRoss()->GetUnitMeanNum(UnitNo));
	OpenPocket(UnitNos, CString(" dictionary entry ") + S);
};


struct CStatis {
	BYTE  DomNo;
	LONG  ItemNo;
	int Freq;
	bool operator==(const CStatis& X) const
	{
		return     ItemNo == X.ItemNo;
	}

	bool operator<(const CStatis& X) const
	{
		return  (ItemNo < X.ItemNo);
	}
};


void CWordList::OnChangeTitle()
{
	if (!GlobalPocketAndArticleDocTempalteAreEmpty(GetDocument())) return;

	uint16_t UnitNo;
	if (!GetSelectedUnitNo(UnitNo)) return;
	std::string unitStr = GetRoss()->GetEntryStr(UnitNo);
	int i = GetRoss()->LocateUnit(unitStr.c_str(), 2);
	if (i != ErrUnitNo)
	{
		AfxMessageBox(_T("You cannot change entry name, if this entry has many senses"));
		return;
	};

	char s[1000];
	strcpy(s, unitStr.c_str());
	if (!InputBoxUtf8("Edit entry name:", s, 1000))
		return;
	if (s == unitStr) return;

	i = GetRoss()->LocateUnit(s, 1);
	if (i != ErrUnitNo)
	{
		AfxMessageBox(_T("This entry name already exists"));
		return;
	};

	GetRoss()->SetUnitStr(UnitNo, s);
	GetDocument()->SetModifiedFlag();
	BuildIndex();
	Update();
}

void CWordList::OnComments()
{
	uint16_t UnitNo;
	if (!GetSelectedUnitNo(UnitNo)) return;
	if (GetDocument()->m_ReadOnly) return;

	const TUnitComment* pComms = GetRoss()->GetCommentsByUnitId(GetRoss()->GetEntries()[UnitNo].m_EntryId);
	char s[100];
	strcpy(s, pComms->Comments);
	if (!InputBoxUtf8("Comments:", s, 100, "ROSS", this))
		return;
	if (!strcmp(s, pComms->Comments)) return;
	GetRoss()->SetUnitCommentStr(UnitNo, s);
	GetDocument()->SetModifiedFlag();
	Update();

};

inline bool GlobalOpenHierarchy(CRossDoc* pRossDoc, CHierarchyEnum Type)
{
	if (pRossDoc == nullptr) {
		AfxMessageBox(_T("please, open the main Russian dictionary"));
		return false;
	}
	else {
		CDocument* pDocument = GetHierarchyTemplate()->CreateNewDocument();
		ASSERT_VALID(pDocument);
		try {
			((CHierarchyDoc*)pDocument)->OpenHierarchy(pRossDoc, Type);
		}
		catch (CExpc& e) {
			AfxMessageBox(_U16(e.what()));
		}
		return true;
	}
};


// иерархия отношений
void CWordList::OnMenuitem32788()
{
	CRossDoc* pRossDoc = ((CRossDevApp*)AfxGetApp())->FindRossDoc(Ross);
	GlobalOpenHierarchy(pRossDoc, SemRel);
}

void CWordList::OnSemFet()
{
	CRossDoc* pRossDoc = ((CRossDevApp*)AfxGetApp())->FindRossDoc(Ross);
	GlobalOpenHierarchy(pRossDoc, SemFet);
}


void CWordList::BuildVals(std::vector<Valency>& Vals, uint16_t UnitNo)
{
	Vals.clear();
	if (GetRoss()->IsEmptyArticle(UnitNo)) return;
	for (size_t k = GetRoss()->GetUnitStartPos(UnitNo); k <= GetRoss()->GetUnitLastPos(UnitNo); k++)
		if (GetRoss()->GetCortege(k).m_FieldNo == GetDocument()->GetRossHolder()->ValFieldNo)
		{
			Valency V;
			V.ValNo = GetRoss()->GetCortege(k).GetItem(0);
			V.A_C = GetRoss()->GetCortege(k).GetItem(1) != GetDocument()->GetRossHolder()->SelfLabelNo;
			Vals.push_back(V);
		};
};

struct CFieldValue {
	TCortege cortege;
	BYTE FieldNo;
	int      freq;

	bool operator==(const CFieldValue& X) const
	{
		TCortege C = cortege;
		TCortege XX = X.cortege;
		return		(FieldNo == X.FieldNo)
			&& C.HasEqualItems(XX);
	}

	bool operator<	(const CFieldValue& X) const
	{
		return  (FieldNo < X.FieldNo) && !cortege.HasEqualItems(X.cortege);
	}

};

void CWordList::OnStatisticFieldValue()
{
	std::vector<CFieldValue> V;
	char s[200];
	s[0] = 0;
	if (!InputBoxUtf8("Enter field name (* for all fields):", s, 200)) return;
	BYTE FieldNo = ErrUChar;
	if (s[0] != '*')
	{
		FieldNo = GetRoss()->GetFieldNoByFieldStr(s);
		if (FieldNo == ErrUChar)
		{
			AfxMessageBox(_T("Cannot find this field"));
			return;
		};
	};

	CWaitCursor C;

	for (size_t i = 0; i < GetRoss()->_GetCortegesSize(); i++)
	{
		if ((FieldNo != ErrUChar)
			&& (GetRoss()->GetCortege(i).m_FieldNo != FieldNo)
			)
			continue;


		CFieldValue Value;
		Value.cortege = GetRoss()->GetCortege(i);
		Value.freq = 1;
		Value.FieldNo = GetRoss()->GetCortege(i).m_FieldNo;
		auto It = find(V.begin(), V.end(), Value);
		if (It == V.end())
			V.push_back(Value);
		else
			It->freq++;
	};

	sort(V.begin(), V.end());
	CString S = "_______________________________________________________\r\n";

	for (int i = 0; i < V.size(); i++)
	{
		BYTE FieldNo = V[i].FieldNo;

		CString Q;
		Q.Format(_T("%-16s %-5s %-30s %-5s %-16i\r\n"),
			_U16(GetRoss()->Fields[V[i].FieldNo].FieldStr), "|",
			_U16(GetRoss()->WriteToString(V[i].cortege)), "|", 
			V[i].freq);
		S += Q;
	};

	GlobalOpenReport(S, "Dictionary fields");


};



void CWordList::OnSortByLemma()
{
	if (GetDocument()->IsThesRoss()) return;
	m_Menu.CheckMenuItem(ID_SORT_BY_LEMMA, MF_BYCOMMAND | MF_CHECKED);
	m_Menu.CheckMenuItem(ID_SORT_BY_DATEANDTIME, MF_BYCOMMAND | MF_UNCHECKED);
	if (m_Sorted == SortByLemma) return;
	m_Sorted = SortByLemma;
	m_LemmaLocatorCtrl.EnableWindow(TRUE);


	Update();
};


void CWordList::OnSortByDateAndTime()
{
	if (GetDocument()->IsThesRoss()) return;
	m_Menu.CheckMenuItem(ID_SORT_BY_LEMMA, MF_BYCOMMAND | MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SORT_BY_DATEANDTIME, MF_BYCOMMAND | MF_CHECKED);
	if (m_Sorted == SortByDateAndTime) return;
	m_Sorted = SortByDateAndTime;
	m_LemmaLocatorCtrl.EnableWindow(FALSE);
	Update();

};





int CompareIndex(const void* arg1, const void* arg2)
{
	CIndex* C1 = (CIndex*)arg1;
	CIndex* C2 = (CIndex*)arg2;
	if (*C1 < *C2)
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


void UpdateWordAndFreq(std::vector <CWordAndFreq>& Unfound, const std::string& Word)
{
	CWordAndFreq W;
	W.m_Word = Word;
	W.m_Freq = 1;
	std::vector<CWordAndFreq>::iterator It = lower_bound(Unfound.begin(), Unfound.end(), W);
	size_t i = It - Unfound.begin();
	if ((It == Unfound.end())
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
		uint16_t MainUnitNo;
		if (!GetSelectedUnitNo(MainUnitNo)) return;

		CTempArticle A1(GetRoss());
		if (MainUnitNo != ErrUnitNo)
			A1.ReadFromDictionary(MainUnitNo, false, false);

		CTempArticle A2(GetRoss());

		CString S;
		if (::MessageBox(this->m_hWnd, _T("The environment add the current entry to all entries, if there is no intersection by fields. Procees?"),
			_T("Confirmation"), MB_YESNO) == IDNO) return;

		for (size_t i = 0; i < GetUnitsSize(); i++)
		{
			uint16_t UnitNo = GetUnitNo(i);
			if (MainUnitNo == UnitNo) continue;
			A2.ReadFromDictionary(UnitNo, false, false);

			A2.AddArticle(&A1);
			A2.WriteToDictionary();
		};
	}
	catch (std::exception& a)
	{
		AfxMessageBox(_U16(a.what()));
	}
	catch (...)
	{
		AfxMessageBox(_T("Some error has occured"));
	};
	
};


struct TUnitInfo {
	std::string UnitStr;
	BYTE MeanNum;
};

void CWordList::OnDelAllSelected()
{
	try {
		CString Q;
		Q.Format(_T("You are going to delete all entries (%i entries). Proceed?"),
			GetUnitsSize());
		if (::MessageBox(this->m_hWnd, Q, _T("Confirmation"), MB_YESNO) == IDNO) return;
		CWaitCursor C;
		std::vector <TUnitInfo> UnitNos;

		for (size_t i = 0; i < GetUnitsSize(); i++)
		{
			size_t u = GetUnitNo(i);
			TUnitInfo U;
			U.UnitStr = GetRoss()->GetEntryStr(u);
			U.MeanNum = GetRoss()->GetUnitMeanNum(u);
			UnitNos.push_back(U);

		};

		for (auto& unit_info: UnitNos)
		{
			uint16_t UnitNo = GetRoss()->LocateUnit(unit_info.UnitStr.c_str(), unit_info.MeanNum);
			GetRoss()->DelUnit(UnitNo);

		};

		BuildIndex();
		Update();
		GetDocument()->SetModifiedFlag();
	}
	catch (...)
	{
		AfxMessageBox(_T("An exception occured!"));

	};
};

void CWordList::OnSetAuthor()
{
	if (GetDocument()->m_ReadOnly) return;
	CString Q;
	Q.Format(_T("You are going to change the author of all entries(%i entries). Proceed?"),
		GetUnitsSize());
	if (::MessageBox(this->m_hWnd, Q, _T("Confirmation"), MB_YESNO) == IDNO) return;
	char s[200];
	s[0] = 0;
	if (!InputBoxUtf8("Enter author name:", s, 15)) return;
	CWaitCursor C;
	std::vector <TUnitInfo> UnitNos;

	for (size_t i = 0; i < GetUnitsSize(); i++)
	{
		size_t u = GetUnitNo(i);
		GetRoss()->SetUnitAuthor(u, s);
	};
	Update();
	GetDocument()->SetModifiedFlag();
};

void CWordList::OnSelectByAuthor()
{
	char s[200];
	s[0] = 0;
	if (!InputBoxUtf8("Input author name:", s, 15)) return;
	CWaitCursor C;

	for (size_t i = 0; i < GetUnitsSize(); i++)
	{
		auto& u = GetRoss()->GetEntries()[GetUnitNo(i)];
		auto author = u.GetAuthorStr();
		if (author == s)
			u.m_bSelected = true;

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
	std::vector<long> Units;
	GxiStatistic(CString SemRelName, TCortege GXi, long UnitNo, BYTE MaxNumDom)
	{
		m_SemRelName = SemRelName;
		m_GXi = GXi;
		m_num = 1;
		m_MaxNumDom = MaxNumDom;
		Units.push_back(UnitNo);
	};
	bool operator<(const GxiStatistic& GX) const
	{
		if (m_SemRelName < GX.m_SemRelName)
			return true;

		if (m_SemRelName > GX.m_SemRelName)
			return false;

		if (m_SemRelName == GX.m_SemRelName)
			if (m_num > GX.m_num)
				return true;

		return false;
	}
	bool operator==(const GxiStatistic& GX) const
	{
		return (m_SemRelName == GX.m_SemRelName) && m_GXi.HasEqualItems(GX.m_GXi);
	};

};


void CWordList::OnSelectDownward()
{


	POSITION pos = m_WordList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		::MessageBox(0, _T("No selection found"), _T("Message Box"), MB_OK);
		return;
	};

	for (size_t i = m_WordList.GetNextSelectedItem(pos); i < m_WordList.GetItemCount(); i++)
	{
		uint16_t UnitNo = GetUnitNo(i);
		GetRoss()->GetEntries()[UnitNo].m_bSelected = true;
	};
	Update();

};


void CWordList::OnEmptyArticles()
{
	try {
		CString Q;
		Q.Format(_T("You are going to clear all entries (%i entries). Proceed?"), GetUnitsSize());
		if (::MessageBox(this->m_hWnd, Q, _T("Confirmation"), MB_YESNO) == IDNO) return;
		CWaitCursor C;
		std::vector <TUnitInfo> UnitNos;

		for (size_t i = 0; i < GetUnitsSize(); i++)
		{
			size_t u = GetUnitNo(i);
			TUnitInfo U;
			U.UnitStr = GetRoss()->GetEntryStr(u);
			U.MeanNum = GetRoss()->GetUnitMeanNum(u);
			UnitNos.push_back(U);
		};

		for (int i = 0; i < UnitNos.size(); i++)
		{
			uint16_t UnitNo = GetRoss()->LocateUnit(UnitNos[i].UnitStr.c_str(), UnitNos[i].MeanNum);
			GetRoss()->ClearUnit(UnitNo);
		};

		BuildIndex();
		Update();
		GetDocument()->SetModifiedFlag();
	}
	catch (...)
	{
		AfxMessageBox(_T("An exception occured!"));
	};
};

void CWordList::OnReload()
{
	if (GetDocument()->IsModified())
	{
		if (::MessageBox(0, _T("You are going to lose all changes, which you have made"),
			_T("Confirmation"), MB_OKCANCEL) != IDOK)
			return;
	};

	if (!GetDocument()->m_ReadOnly)
		GetDocument()->RemoveLock();

	CFile F(GetDocument()->GetPathName(), CFile::modeRead);
	CArchive ar(&F, CArchive::load);
	GetDocument()->Serialize(ar);
	BuildIndex();
	Update();
	GetDocument()->SetModifiedFlag(FALSE);
	GetDescendantWindow(IDC_READONLY_IMAGE)->ShowWindow(GetDocument()->m_ReadOnly ? SW_SHOW : SW_HIDE);

};

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
	uint16_t UnitNo;
	if (pos != 0)
	{
		UnitNo = m_WordList.GetNextSelectedItem(pos);
		m_UnitsSize.Format(_T("Entry No %i (out of %i)"),
			UnitNo + 1, m_WordList.GetItemCount());
		UpdateData(FALSE);
	};

};





void CWordList::OnClickWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateCurrentPos();
	m_UnitSizeCtrl.Invalidate();

	*pResult = 0;
}

void CWordList::OnDblclkWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
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
	if ((message == 273) && (wParam == 1))
	{
		CWnd* FocusWnd = GetFocus();
		if ((FocusWnd == &m_WordList)
			|| (FocusWnd == &m_LemmaLocatorCtrl)
			)
			OnWordlistArticleBtn();
	}



	return CSizeFormView::DefWindowProc(message, wParam, lParam);
}


bool UnlockAllDicts()
{
	try {
		CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
		POSITION pos = pRossDocTemplate->GetFirstDocPosition();
		int RossNo = 0;
		while (pos)
		{
			CDocument* pDoc = pRossDocTemplate->GetNextDoc(pos);
			POSITION pos1 = pDoc->GetFirstViewPosition();
			if (pos1 == 0) return false;

			CWordList* pView = (CWordList*)pDoc->GetNextView(pos1);
			int i = 0;
			while (pView->GetDocument()->m_ReadOnly)
			{
				CString S;
				S.Format(_T("The dictionary %s is locked. Delete lock file !"), pView->GetDocument()->GetRossHolder()->GetDictName().c_str());
				if (i == 1)
					AfxMessageBox(S);

				pView->OnReload();
				if (i == 1)
				{
					S.Format(_T("The dictionary %s is locked. Exit from the procedure!"), pView->GetDocument()->GetRossHolder()->GetDictName().c_str());
					return false;
				};
				i++;
			};
			CString FileName;
		};
	}
	catch (...)
	{
		AfxMessageBox(_T("An exception occured!"));
		return false;
	};
	return true;

}


struct CDictInterp {
	std::string					m_UnitStr;
	BYTE					m_MeanNum;
	const CWordList* m_Owner;
	bool operator <  (const CDictInterp& X) const
	{
		if (m_UnitStr != X.m_UnitStr)
			return m_UnitStr < X.m_UnitStr;

		if (m_Owner == X.m_Owner)
			return m_MeanNum < X.m_MeanNum;
		else
			return m_Owner < X.m_Owner;
	};
};

void AddEntries(const CWordList* WordList, std::vector<CDictInterp>& AllEntries)
{
	if (!WordList->GetDocument()->IsThesRoss())
	{
		for (size_t i = 0; i < WordList->GetUnitsSize(); i++)
		{
			size_t u = WordList->GetUnitNo(i);
			CDictInterp I;
			I.m_UnitStr = WordList->GetRoss()->GetEntryStr(i);
			I.m_MeanNum = WordList->GetRoss()->GetUnitMeanNum(u);
			I.m_Owner = WordList;
			AllEntries.push_back(I);
		}
	}
	else
	{
		int ThesId = WordList->GetDocument()->GetThesId();
		const CThesaurus* Thes = GetSemBuilder().m_RusStr.m_pData->GetThes(ThesId);
		for (int i = 0; i < Thes->m_Termins.size(); i++)
		{
			CDictInterp I;
			I.m_UnitStr = Thes->m_Termins[i].m_TerminStr;
			I.m_MeanNum = 1;
			I.m_Owner = WordList;
			AllEntries.push_back(I);
		};
	};
};
