// MorphwizardView.cpp : implementation of the CMorphwizardView class
//
#include "stdafx.h"

#include "Morphwizard.h"

#include "MorphwizardDoc.h"
#include "MorphwizardView.h"
#include "GriIni.h"
#include "DlgDelAnc.h"
#include "SLFDocument.h"

#include "ProgressBar.h"
#include "AccentWizardDlg.h"
#include "InputBox.h"
#include <fstream>

//----------------------------------------------------------------------------
CDocTemplate* GetTemplate(CString Name)
{
	// получение template документа статьи
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* tmpl;
	CString S;
	do {
		tmpl = AfxGetApp()->GetNextDocTemplate(pos);
		tmpl->GetDocString(S, CDocTemplate::regFileTypeId);
	} while ((tmpl != 0) && (S != Name));

	ASSERT(tmpl);

	return tmpl;
}


IMPLEMENT_DYNCREATE(CMorphwizardView, CFormView)

BEGIN_MESSAGE_MAP(CMorphwizardView, CFormView)
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_PREDICT, OnPredict)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_EN_SETFOCUS(IDC_FIND_WHAT, OnSetfocusFindWhat)
	ON_EN_KILLFOCUS(IDC_FIND_WHAT, OnKillfocusFindWhat)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PREDICTED_LIST, OnColumnclickPredictedList)
	ON_NOTIFY(NM_DBLCLK, IDC_PREDICTED_LIST, OnDblclkPredictedList)
	ON_EN_SETFOCUS(IDC_PREDICT_WHAT, OnSetfocusPredictWhat)
	ON_EN_KILLFOCUS(IDC_PREDICT_WHAT, OnKillfocusPredictWhat)
	ON_WM_HELPINFO()
	ON_NOTIFY(NM_DBLCLK, IDC_FOUND_LIST2, OnNMDblclkFoundList2)
	ON_COMMAND(ID_FIND_PARADIGM, OnFindParadigm)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_EN_KILLFOCUS(IDC_FILTER, OnEnKillfocusFilter)
	ON_EN_SETFOCUS(IDC_FILTER, OnEnSetfocusFilter)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FOUND_LIST2, OnLvnColumnclickFoundList2)
	ON_COMMAND(ID_TOOLS_SAVE_LIST_FILE, OnToolsSaveListFile)
	ON_COMMAND(ID_TOOLS_IMPORT, OnToolsImport)
	ON_COMMAND(ID_TOOLS_EXPORT, OnToolsExport)
	ON_COMMAND(ID_TOOLS_PACK_MRD, OnToolsPackMrd)
	ON_COMMAND(ID_TOOLS_SETPARADIGMNO, OnToolsSetParaNo)
	ON_COMMAND(ID_TOOLS_SET_ACCENT_MODEL_NO, OnToolsSetAccentModelNo)
	ON_COMMAND(ID_TOOLS_SELECT_BY_FILE, OnToolsSelectByFile)
	ON_COMMAND(ID_TOOLS_SHOW_PARA_DIFF, OnToolsShowparadigmdifferences)
	ON_COMMAND(ID_SHOW_ACCENT_MODEL_DIFF, OnToolsShowAccentDifferences)
	
	ON_COMMAND(ID_TOOLS_ACCENTWIZARD, OnToolsAccentWizard)
	ON_MESSAGE(WM_NEXT_NONACCENTED_PARA, OnNextNonAccentedPara)
	ON_BN_CLICKED(IDC_SET_PRD_COMMENTS, OnBnClickedSetPrdComments)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CMorphwizardView::DoDataExchange(CDataExchange* pDX)
{
	CView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREDICTED_LIST, m_PredictedList);
	DDX_Control(pDX, IDC_PREDICT_WHAT, m_PredictWhat);
	DDX_Control(pDX, IDC_FIND_WHAT, m_FindWhat);
	DDX_Control(pDX, IDC_STATUS, m_StatusCtrl);
	DDX_Text(pDX, IDC_STATUS, m_Status);
	DDX_Text(pDX, IDC_MINIMAL_FREQUENCE, m_MinimalFrequence);
	DDX_Check(pDX, IDC_ONLY_MAIN_PART_OF_SPEECHES, m_bOnlyMainPartOfSpeeches);
	DDX_Control(pDX, IDC_FOUND_LIST2, m_FoundList);
	DDX_Text(pDX, IDC_SUF_EDIT, m_PredictSuffLength);
	DDV_MinMaxInt(pDX, m_PredictSuffLength, 2, 5);

	//  init history
	m_FindWhat.ResetContent();
	std::list<CString>::iterator pos = m_LastQueries.begin();
	while (pos != m_LastQueries.end())
	{
		m_FindWhat.AddString(*pos);
		pos++;
	}
	m_FindWhat.SetCurSel(0);


}

CMorphwizardView::CMorphwizardView() :CFormView(CMorphwizardView::IDD)
, m_PredictSuffLength(2)
{
	m_SortPredictColumn = -1;
	LoadHistory();
	m_ControlMargin = 10;
}

CMorphwizardView::~CMorphwizardView()
{
	SaveHistory();
}

MorphoWizard* CMorphwizardView::GetWizard()
{
	return  &(GetDocument()->m_Wizard);
};


void CMorphwizardView::OnDraw(CDC* /*pDC*/)
{
	CMorphwizardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}


#ifdef _DEBUG
void CMorphwizardView::AssertValid() const
{
	CView::AssertValid();
}

void CMorphwizardView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMorphwizardDoc* CMorphwizardView::GetDocument() const {
	// non-debug version is inline
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMorphwizardDoc)));
	return (CMorphwizardDoc*)m_pDocument;
}
#endif //_DEBUG


const size_t FindParadigmColumnNo = 0;
const size_t FindLemmaColumn = 1;
const size_t FindGrammemsColumn = 2;
const size_t FindTypeGrammemsColumn = 3;



void CMorphwizardView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// sizing 
	int GlobalX = ::GetSystemMetrics(SM_CXSCREEN);
	int GlobalY = ::GetSystemMetrics(SM_CYSCREEN);
	const  int DlgWidth = 830;
	const  int DlgHeight = 600;
	GetParent()->SetWindowPos(NULL, (GlobalX - DlgWidth) / 2, (GlobalY - DlgHeight) / 2 - 60, DlgWidth, DlgHeight, SWP_SHOWWINDOW | SWP_NOZORDER);
	GetParent()->ShowWindow(SW_MAXIMIZE); // Nick [12/Dec/2003]

	m_PredictedList.InsertColumn((int)TLemmPredictSortEnum::Grammems, _T("Grammems"), LVCFMT_LEFT, 90);
	m_PredictedList.InsertColumn((int)TLemmPredictSortEnum::TypeDict, _T("Type"), LVCFMT_LEFT, 35);
	m_PredictedList.InsertColumn((int)TLemmPredictSortEnum::Lemma, _T("Source lemma"), LVCFMT_LEFT, 90);
	m_PredictedList.InsertColumn((int)TLemmPredictSortEnum::Freq, _T("Freq"), LVCFMT_LEFT, 50);
	m_PredictedList.InsertColumn((int)TLemmPredictSortEnum::Index, _T("Innernumber"), LVCFMT_LEFT, 0);
	m_PredictedList.InsertColumn((int)TLemmPredictSortEnum::Paradigm, _T("ParadigmNo"), LVCFMT_LEFT, 50);
	m_PredictedList.InsertColumn((int)TLemmPredictSortEnum::LemmaPrefix, _T("Prefix"), LVCFMT_LEFT, 50);
	m_PredictedList.InsertColumn((int)TLemmPredictSortEnum::FormsCount, _T("Forms Count"), LVCFMT_LEFT, 40);
	m_PredictedList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_FoundList.InsertColumn(FindParadigmColumnNo, _T("ParadigmNo"), LVCFMT_LEFT, 50);
	m_FoundList.InsertColumn(FindLemmaColumn, _T("Lemma"), LVCFMT_LEFT, 130);
	m_FoundList.InsertColumn(FindGrammemsColumn, _T("Grammems"), LVCFMT_LEFT, 210);
	m_FoundList.InsertColumn(FindTypeGrammemsColumn, _T("Type"), LVCFMT_LEFT, 210);
	m_FoundList.SetExtendedStyle(LVS_EX_FULLROWSELECT);


	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CheckRadioButton(IDC_RFIND_LEM, IDC_WORD_FORM, IDC_RFIND_LEM);
	m_inFind = true;
	m_inPredict = false;
	m_inFilter = false;

	if (!GetWizard()->m_ReadOnly)
	{
		GetDescendantWindow(IDC_READONLY_IMAGE)->ShowWindow(SW_HIDE);
	}

	m_MinimalFrequence = 2;
	m_bOnlyMainPartOfSpeeches = TRUE;
	UpdateData(FALSE);

}

HCURSOR CMorphwizardView::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void CMorphwizardView::echo(CString s)
{
	m_StatusCtrl.SetWindowText(s);
}

void CMorphwizardView::FilterFoundParadigms()
{
	CString tmp;
	GetDlgItem(IDC_FILTER)->GetWindowText(tmp);
	std::string flt_str = ToInnerEncoding(tmp);
	Trim(flt_str);
	if (flt_str.empty()) return;
	std::string common_ancode;
	if (isdigit((BYTE)flt_str[0]))
	{
		if (strspn(flt_str.c_str(), "0123456789") != flt_str.length())
		{
			ErrorMessage("Bad format for paradigm No in filter");
			return;
		};
		size_t FlexiaModel = atoi(flt_str.c_str());
		if (FlexiaModel >= GetWizard()->m_FlexiaModels.size())
		{
			ErrorMessage("Paradigm No in filter is too high ");
			return;
		};
		std::vector<lemma_iterator_t> filter_paradigms;
		for (int i = 0; i < found_paradigms.size(); i++)
		{
			if (found_paradigms[i]->second.m_FlexiaModelNo != FlexiaModel) continue;
			filter_paradigms.push_back(found_paradigms[i]);
		}
		found_paradigms = filter_paradigms;

	}
	else
	{
		if (!GetWizard()->Filter(flt_str, found_paradigms)) {
			AfxMessageBox(IDS_BADFSTR, MB_ICONERROR | MB_OK);
		}
	};

};

void CMorphwizardView::ShowFoundParadigms()
{
	m_FoundList.DeleteAllItems();

	for (int i = 0; i < found_paradigms.size(); i++)
	{
		int FlexiaModelNo = found_paradigms[i]->second.m_FlexiaModelNo;
		CString S;
		S.Format(_T("%i (%i)"), FlexiaModelNo, found_paradigms[i]->second.m_AccentModelNo);
		m_FoundList.InsertItem(i, S);

		std::string Lemma = GetWizard()->get_lemm_string_with_accents(found_paradigms[i]);
		std::string PrefixSet = GetWizard()->get_prefix_set(found_paradigms[i]);
		if (!PrefixSet.empty())
			Lemma = PrefixSet + '|' + Lemma;
		RmlMakeLower(Lemma, GetWizard()->m_Language);
		m_FoundList.SetItemText(i, FindLemmaColumn, FromInnerEncoding(Lemma));

		auto s = GetWizard()->get_pos_string(found_paradigms[i]) + " " + GetWizard()->get_grammem_string(found_paradigms[i]);
		m_FoundList.SetItemText(i, FindGrammemsColumn, FromInnerEncoding(s));

		s = GetWizard()->get_common_grammems_string(found_paradigms[i]);
		m_FoundList.SetItemText(i, FindTypeGrammemsColumn, FromInnerEncoding(s));
	}

	char ch[1024];
	sprintf(ch, "Found %d paradigms", found_paradigms.size());
	echo(ch);
};

void CMorphwizardView::OnFind()
{
	try
	{
		m_FoundList.DeleteAllItems();
		found_paradigms.clear();
		m_FindWhat.SetFocus();

		CString find_what;
		m_FindWhat.GetWindowText(find_what);
		find_what.Trim();
		if (find_what == "") return;
		ChangeHistory(find_what);
		UpdateData(FALSE);
		std::string find_what_u8 = utf16_to_utf8((wchar_t*)find_what);
		Trim(find_what_u8);

		CWizardProgressMeter meter(*GetWizard());
		int typeSearch = GetCheckedRadioButton(IDC_FIRST_BUTTON_SEARCH_TYPE, IDC_LAST_BUTTON_SEARCH_TYPE);
		switch (typeSearch) {
		case IDC_RFIND_LEM:
			found_paradigms = GetWizard()->find_lemm(find_what_u8.c_str(), false);
			break;
		case IDC_FIND_BY_PARADIGM_NO:
			if (isdigit(find_what_u8[0])) {
				GetWizard()->find_lemm_by_prdno(atoi(find_what_u8.c_str()), found_paradigms);
			}
			break;
		case IDC_RFIND_GRM:
			GetWizard()->find_lemm_by_grammem(find_what_u8, found_paradigms);
			break;
		case IDC_WORD_FORM:
			MakeUpperUtf8(find_what_u8);
			found_paradigms = GetWizard()->find_wordforms(find_what_u8);
			break;
		case IDC_FIND_BY_GRAMCODE:
			GetWizard()->find_ancodes(find_what_u8, found_paradigms);
			break;
		case IDC_FIND_BY_USERNAME:
			GetWizard()->find_lemm_by_user(find_what_u8, found_paradigms);
			break;
		case IDC_FIND_BY_ACCENT_MODEL:
			if (isdigit(find_what_u8[0])) {
				GetWizard()->find_lemm_by_accent_model(atoi(find_what_u8.c_str()), found_paradigms);
			}
			break;
		}

		FilterFoundParadigms();
		ShowFoundParadigms();
	}
	catch (CExpc c)
	{
		ErrorMessage(c.what());
	}
	catch (...) {
		AfxMessageBox(_T("Search failed"));
	}
}



void CMorphwizardView::OnRemove()
{
	try
	{
		POSITION pos = m_FoundList.GetFirstSelectedItemPosition();
		if (pos == 0)
		{
			echo("No item selected");
		}
		else
		{
			size_t index = m_FoundList.GetNextSelectedItem(pos);
			GetWizard()->remove_lemm(found_paradigms.at(index));
			echo("Word removed");
		}
		m_FoundList.DeleteAllItems();
		OnFind();
	}
	catch (...)
	{
		ErrorMessage("An exception occurred!");
	};
}


std::string CMorphwizardView::ToInnerEncoding(CString strText) const
{
	return utf16_to_utf8((LPCTSTR)strText);
}

CString CMorphwizardView::FromInnerEncoding(std::string s) const
{
	return utf8_to_utf16(s).c_str();
}


void CMorphwizardView::OnPredict()
{
	try
	{
		UpdateData(TRUE);
		m_FoundList.DeleteAllItems();
		std::vector<lemma_iterator_t> curr_found_paradigms;

		m_PredictedList.DeleteAllItems();

		CString predict_what;
		CString prefix;

		m_PredictWhat.GetWindowText(predict_what);
		std::string Lemma = ToInnerEncoding(predict_what);
		int i = predict_what.Find(_T("#"));

		if (i != -1)
		{
			prefix = predict_what.Mid(0, i);
			predict_what = predict_what.Mid(i + 1);
			Lemma = ToInnerEncoding(prefix + predict_what);
		};

		GetWizard()->find_lemm(Lemma.c_str(), false, curr_found_paradigms);
		if (!curr_found_paradigms.empty())
			AfxMessageBox(_T("The dictionary contains already this word!"));
		m_LemmaToPredict = ToInnerEncoding(predict_what);
		m_PredictedParadigms =  GetWizard()->m_Predictor.predict_lemm(
			m_LemmaToPredict,
			m_PredictSuffLength,
			m_MinimalFrequence,
			m_bOnlyMainPartOfSpeeches == TRUE,
			(TLemmPredictSortEnum)m_SortPredictColumn);

		if (m_PredictedParadigms.empty())
		{
			ErrorMessage("No variants found");
			return;
		};

		for (size_t i = 0; i < m_PredictedParadigms.size(); i++)
		{
			const CPredictSuffix& S = m_PredictedParadigms[i];
			const CFlexiaModel& P = GetWizard()->m_FlexiaModels[S.m_FlexiaModelNo];

			m_PredictedList.InsertItem(i,
				FromInnerEncoding(GetWizard()->get_pos_string_and_grammems(S.m_SourceLemmaAncode)));
			m_PredictedList.SetItemText(i,
				(int)TLemmPredictSortEnum::TypeDict,
				FromInnerEncoding(GetWizard()->get_grammem_string(S.m_SourceCommonAncode)));
			m_PredictedList.SetItemText(i, (int)TLemmPredictSortEnum::Lemma, FromInnerEncoding(S.m_SourceLemma));
			m_PredictedList.SetItemText(i, (int)TLemmPredictSortEnum::Freq, IntToStr(S.m_Frequence));
			m_PredictedList.SetItemText(i, (int)TLemmPredictSortEnum::Index, IntToStr(i));
			m_PredictedList.SetItemText(i, (int)TLemmPredictSortEnum::Paradigm, IntToStr(S.m_FlexiaModelNo));
			m_PredictedList.SetItemText(i, (int)TLemmPredictSortEnum::LemmaPrefix, FromInnerEncoding(S.m_PrefixSetStr));
			m_PredictedList.SetItemText(i, (int)TLemmPredictSortEnum::FormsCount, IntToStr(P.m_Flexia.size()));
		}
		OnFind();
		m_PredictWhat.SetFocus();
	}
	catch (...)
	{
		ErrorMessage("wizard", "an exception occurred!");
	};

}

CSLFDocument* CMorphwizardView::NewSLFDocument()
{
	CSLFDocument* pDocument = (CSLFDocument*)GetTemplate("MorphWizard.slf")->CreateNewDocument();
	pDocument->m_pParent = this;
	ASSERT_VALID(pDocument);
	return pDocument;
};

void CMorphwizardView::OnAdd()
{
	POSITION pos = m_PredictedList.GetFirstSelectedItemPosition();

	try {
		CSLFDocument* pDocument = NewSLFDocument();

		CString predict_what;
		m_PredictWhat.GetWindowText(predict_what);
		if (pos == NULL)
		{
			predict_what += " ";
			pDocument->m_ParadigmText = predict_what;
			pDocument->m_CommonGrammems = "";
		}
		else
		{
			size_t ind = m_PredictedList.GetNextSelectedItem(pos);
			CString S = m_PredictedList.GetItemText(ind, (int)TLemmPredictSortEnum::Index);
			auto slf = GetWizard()->create_slf_for_lemm(m_LemmaToPredict, m_PredictedParadigms[ind].m_FlexiaModelNo, 50);
			pDocument->m_ParadigmText = FromInnerEncoding(slf);
			std::string common_grammems = GetWizard()->get_grammem_string(m_PredictedParadigms[ind].m_SourceCommonAncode.c_str());
			pDocument->m_CommonGrammems = FromInnerEncoding(common_grammems);
			int i = predict_what.Find(_T("|"));
			if (i != -1)
				pDocument->m_Prefixes = predict_what.Mid(0, i);
		}


		pDocument->OpenParadigm();

	}
	catch (CExpc& e) {
		echo(e.what());
	}
	catch (...) {
		echo("error");
	}
}

//----------------------------------------------------------------------------
bool CMorphwizardView::OpenExistingParadigm(lemma_iterator_t it, bool bRunAccentWizard)
{
	try
	{
		if (it == GetWizard()->m_LemmaToParadigm.end())
			return false;

		std::string save_common_grammems, save_slf, save_prefixes;
		save_slf = GetWizard()->get_slf_string(it, save_common_grammems, save_prefixes, 50);

		CSLFDocument* pDocument = NewSLFDocument();
		pDocument->m_Paradigm = it->second;
		pDocument->m_ParadigmText = FromInnerEncoding(save_slf);
		pDocument->m_CommonGrammems = FromInnerEncoding(save_common_grammems);
		pDocument->m_Prefixes = FromInnerEncoding(save_prefixes);

		pDocument->OpenParadigm(bRunAccentWizard);
		return true;
	}
	catch (CExpc& e)
	{
		echo(e.what());
	}
	catch (...)
	{
		echo("error");
	}
	return false;
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnEdit()
{
	POSITION pos = m_FoundList.GetFirstSelectedItemPosition();
	if (pos == 0)
	{
		echo("No item selected");
	}
	else
	{
		size_t index = m_FoundList.GetNextSelectedItem(pos);
		echo("Editing");
		OpenExistingParadigm(found_paradigms[index]);
	}
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsSaveListFile()
{
	CFileDialog D(FALSE, _T("txt"), _T("wordlist.txt"));
	if (D.DoModal() != IDOK) return;
	FILE* fp = fopen(utf16_to_utf8((const TCHAR*)D.GetPathName()).c_str(), "wb");

	for (int i = 0; i < found_paradigms.size(); i++)
	{
		std::string lemma = GetWizard()->get_lemm_string_with_accents(found_paradigms[i]);
		std::string PrefixSet = GetWizard()->get_prefix_set(found_paradigms[i]);
		if (!PrefixSet.empty())
			lemma = PrefixSet + '|' + lemma;
		RmlMakeLower(lemma, GetWizard()->m_Language);
		std::string common_grams = "* " + GetWizard()->get_grammem_string(found_paradigms[i]->second.GetCommonAncodeCopy());

		std::string s = lemma
			+ ";"
			+ common_grams
			+ ";"
			+ GetWizard()->get_pos_string(found_paradigms[i])
			+ " "
			+ GetWizard()->get_grammem_string(found_paradigms[i]);
		fprintf(fp, "%s\n", convert_to_utf8(s, GetWizard()->m_Language).c_str());

	}
	fclose(fp);
}

//----------------------------------------------------------------------------
static BOOL remove_ancode(const std::string& str, const std::string& ancode, std::string& res)
{
	res = str;
	int pos1 = str.find(ancode);
	if (pos1 == std::string::npos)
		return(FALSE);
	int pos2 = pos1 + ancode.size();
	int grp1 = str.rfind("*", pos1);
	if (grp1 == std::string::npos)
		return(FALSE);
	int grp2 = str.find("%", pos2);
	if (grp2 == std::string::npos)
		grp2 = str.size();
	if (grp1 + 1 < pos1 || pos2 < grp2)
	{
		res = str.substr(pos1, pos2 - pos1);
		res = str;
		res.erase(pos1, pos2 - pos1);
		return(TRUE);
	}
	int grp0 = str.rfind("%", grp1);
	if (grp0 == std::string::npos)
		return(FALSE);
	res = str.substr(grp0, grp2 - grp0);
	res = str;
	res.erase(grp0, grp2 - grp0);
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////

void CMorphwizardView::OnSetfocusFindWhat()
{
	m_inFind = true;
}

void CMorphwizardView::OnKillfocusFindWhat()
{
	m_inFind = false;
}

/////////////////////////////////////////////////////////////////////////////
void CMorphwizardView::OnToolsExport()
{
	try {
		CFileDialog D(FALSE, _T("slf"), _T("paradigms.slf"));
		if (D.DoModal() != IDOK) return;
		std::ofstream fp(utf16_to_utf8((const TCHAR*)D.GetPathName()).c_str(), std::ios::binary);
		if (!fp.is_open())
		{
			AfxMessageBox(_T("Cannot open file"));
			return;
		};

		CProgressMeterRML meter;
		meter.SetMaxPos(found_paradigms.size(), 100);
		CString info = _T("Exporting ") + D.GetPathName() + _T("...");
		meter.SetInfo(info);

		GetWizard()->set_to_delete_false();
		for (long i = 0; i < found_paradigms.size(); i++)
		{
			CDumpParadigm P(GetWizard());
			P.m_SlfStr = GetWizard()->get_slf_string(found_paradigms[i], P.m_TypeGrammemsStr, P.m_PrefixesStr, 50);

			if (found_paradigms[i]->second.m_PrefixSetNo != UnknownPrefixSetNo)
				P.m_PrefixesStr = GetWizard()->get_prefix_set(found_paradigms[i]);

			if (!found_paradigms[i]->second.GetCommonAncodeCopy().empty())
				P.m_TypeGrammemsStr = GetWizard()->get_grammem_string(found_paradigms[i]->second.GetCommonAncodeCopy());

			if (found_paradigms[i]->second.m_SessionNo != UnknownSessionNo)
				P.m_Session = GetWizard()->get_session(found_paradigms[i]->second.m_SessionNo);
			fp << P.ToStringUtf8();

			found_paradigms[i]->second.m_bToDelete = true;

			meter.AddPos();
		}
		fp.close();

		if (::MessageBox(0, _T("Delete all exported paradigms?"), _T("MorphWizard"), MB_YESNO) == IDYES)
			GetWizard()->delete_checked_lemms();
	}
	catch (...)
	{
		AfxMessageBox(_T("some error has occured"));
	};


}

//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsImport()
{
	int line_no = 0;
	CString PathName;

	try {
		CFileDialog D(TRUE, _T("slf"), _T("paradigms.slf"), OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
		TCHAR file_buffer[10000];
		file_buffer[0] = 0;
		D.m_ofn.lpstrFile = file_buffer;
		D.m_ofn.nMaxFile = 10000;
		if (D.DoModal() != IDOK) return;
		POSITION pos = D.GetStartPosition();
		if (pos == 0) return;
		bool bTestMode = ::MessageBox(0, _T("Test it or import?"), _T("MorphWizard"), MB_YESNO) == IDYES;

		GetWizard()->m_bFullTrace = false;
		PathName = D.GetNextPathName(pos);
		while (true) // all selected files 
		{

			FILE* fp = fopen(utf16_to_utf8((const TCHAR*)PathName).c_str(), "r");
			if (!fp)
			{
				AfxMessageBox(_T("Cannot open file"));
				return;
			};

			LOGI << "import file " << PathName;

			CProgressMeterRML meter;
			meter.SetFileMaxPos(fp);
			CString info = (bTestMode ? "Test" : "Import");
			info += "ing file " + PathName + "...";
			meter.SetInfo(info);

			int ParadigmCount = 0;
			std::string Errors;
			line_no = 0;
			bool bError;
			CDumpParadigm P(GetWizard());
			while (P.ReadNextParadigmFromFile(fp, line_no, bError, Errors))
			{
				if (!bError)
				{
					int line_no_err = 0;
					ParadigmCount++;
					try
					{
						if (bTestMode)
						{
							std::string lemm;
							CFlexiaModel Dummy1;
							CAccentModel AccentModel;
							BYTE Dummy;
							GetWizard()->slf_to_mrd(P.m_SlfStr, lemm, Dummy1, AccentModel, Dummy, line_no_err);
						}
						else
						{
							uint16_t SessionNo = GetWizard()->RegisterSession(P.m_Session);
							GetWizard()->add_lemma_to_dict(P.m_SlfStr, P.m_TypeGrammemsStr, P.m_PrefixesStr, line_no_err, SessionNo);
						};

						meter.SetFilePos();
					}
					catch (std::exception c)
					{
						Errors += Format("%s (%s:%i) \n", c.what(), (const TCHAR*)PathName, P.m_FirstSlfLineNo + line_no_err);
					}
					catch (...)
					{
						Errors += Format("error at %s:%i \n", (const TCHAR*)PathName, P.m_FirstSlfLineNo + line_no_err);
					};

				}
			}
			fclose(fp);
			if (!Errors.empty())
			{
				try {
					std::string ErrorFile = MakeFName(ToInnerEncoding(PathName).c_str(), "err");
					FILE* fp = fopen(ErrorFile.c_str(), "w");
					fprintf(fp, "%s", Errors.c_str());
					fclose(fp);
					ErrorMessage(Format("Errors were written to file %s", ErrorFile.c_str()));
				}
				catch (...) {
					AfxMessageBox(_T("Cannot write errors to paradigms.err "));
				}
			}
			else
			{
				auto mess = Format("Successfully %s %i paradigms from \"%s\"", bTestMode ? "tested" : "imported", ParadigmCount, PathName);
				ErrorMessage("Confirmation", mess);
			};
			if (pos == 0) break;
			PathName = D.GetNextPathName(pos);
		} // all selected files

	}
	catch (...)
	{
		ErrorMessage(Format("some error has occurred (file=%s, line=%i", PathName, line_no));
	};
	GetWizard()->m_bFullTrace = true;
}

//----------------------------------------------------------------------------
//	Ищет слова по заданному в файле перечню. 
//  В файле на каждой строке сначала стоит часть речи, а потом лемма.
//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsSelectByFile()
{
	// TODO: Add your command handler code here
	CFileDialog D(TRUE, _T("slf"), _T("paradigms.txt"));
	if (D.DoModal() != IDOK) return;
	FILE* fp = fopen(utf16_to_utf8((const TCHAR*)D.GetPathName()).c_str(), "rb");
	if (!fp)
	{
		AfxMessageBox(_T("Cannot open file"));
		return;
	};
	char buf[1000];
	std::string strNotFound;
	int ParadigmCount = 0;
	found_paradigms.clear();
	while (fgets(buf, 1000, fp))
	{
		std::string Line = convert_from_utf8(buf, GetWizard()->m_Language);
		Trim(Line);
		if (Line.empty()) continue;
		StringTokenizer tok(Line.c_str(), ";");
		if (!tok())
		{
			std::string mess = std::string("cannot get lemma from ") + buf + std::string("; The format should be <Lemma>;<TypeGrammems>;<MorphPattern>");
			AfxMessageBox(FromInnerEncoding(mess));
			break;
		};
		std::string Lemma = tok.val();
		Trim(Lemma);


		if (!tok())
		{
			std::string mess = std::string("cannot get type grammem ") + buf + std::string("; The format should be <Lemma> <PartofSpeech> <Grammems>");
			AfxMessageBox(FromInnerEncoding(mess));
			break;
		};
		std::string TypeAncode;
		{
			std::string grams = tok.val();
			Trim(grams);
			if (grams != "*")
				if (!GetWizard()->slf2ancode(grams, TypeAncode))
				{
					std::string mess = std::string("cannot process type grammems ") + grams;
					AfxMessageBox(FromInnerEncoding(mess));
					break;
				};
		};


		if (!tok())
		{
			std::string mess = std::string("cannot get morphological pattern ") + buf + std::string("; The format should be <Lemma>;<TypeGrammems>;<MorphPattern>");
			AfxMessageBox(FromInnerEncoding(mess));
			break;
		};
		std::string FirstCode;
		{
			std::string PosStr = tok.val();
			Trim(PosStr);
			if (!GetWizard()->slf2ancode(PosStr, FirstCode))
			{
				std::string mess = std::string("cannot process morph. pattern ") + PosStr;
				AfxMessageBox(FromInnerEncoding(mess));
				break;
			};
		};

		std::vector<lemma_iterator_t> curr_found_paradigms;
		RmlMakeUpper(Lemma, GetWizard()->m_Language);
		bool bFound = false;
		GetWizard()->find_lemm(Lemma.c_str(), true, curr_found_paradigms);
		for (size_t i = 0; i < curr_found_paradigms.size(); i++)
		{
			std::string str_pos = GetWizard()->m_FlexiaModels[curr_found_paradigms[i]->second.m_FlexiaModelNo].get_first_code();;

			if (curr_found_paradigms[i]->second.GetCommonAncodeCopy() == TypeAncode)
				if (FirstCode == str_pos)
					if (std::find(found_paradigms.begin(), found_paradigms.end(), curr_found_paradigms[i]) == found_paradigms.end())
					{
						found_paradigms.push_back(curr_found_paradigms[i]);
						bFound = true;
					};
		}
		if (!bFound)
			strNotFound += Format("Not found: %s\r\n", Line.c_str());


	};
	fclose(fp);

	FilterFoundParadigms();
	ShowFoundParadigms();
	if (!strNotFound.empty())
		echo(strNotFound.c_str());
}

//----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
bool IsLessByParadigmNo(const lemma_iterator_t& _X1, const lemma_iterator_t& _X2)
{
	return  _X1->second.m_FlexiaModelNo < _X2->second.m_FlexiaModelNo;

}

bool IsLessByLemma(const lemma_iterator_t& _X1, const lemma_iterator_t& _X2)
{
	return  _X1->first < _X2->first;

}

//----------------------------------------------------------------------------
void CMorphwizardView::OnLvnColumnclickFoundList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int ColumnNo = pNMLV->iSubItem;
	if (ColumnNo == FindParadigmColumnNo)
	{
		std::sort(found_paradigms.begin(), found_paradigms.end(), IsLessByParadigmNo);
		ShowFoundParadigms();
	}
	else
		if (FindLemmaColumn == ColumnNo)
		{
			std::sort(found_paradigms.begin(), found_paradigms.end(), IsLessByLemma);
			ShowFoundParadigms();
		};

	*pResult = 0;
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnColumnclickPredictedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_SortPredictColumn = pNMListView->iSubItem;
	OnPredict();
	*pResult = 0;

}

void CMorphwizardView::OnDblclkPredictedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnAdd();

	*pResult = 0;
}

void CMorphwizardView::OnSetfocusPredictWhat()
{
	m_inPredict = true;

}

void CMorphwizardView::OnKillfocusPredictWhat()
{
	m_inPredict = false;

}

void CMorphwizardView::OnToolsPackMrd()
{
	CGriIni cIni(_T("Packing..."));;
	cIni.Init(this);
	{
		CWizardProgressMeter meter(*GetWizard());
		GetWizard()->pack();
	}
	AfxMessageBox(_T("Packing is finished!"));
}

BOOL CMorphwizardView::OnHelpInfo(HELPINFO* pHelpInfo)
{
	AfxGetApp()->WinHelp(IDH_MORPH_WIZARD_DLG);
	return TRUE;
}

void CMorphwizardView::OnNMDblclkFoundList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnEdit();
	*pResult = 0;
}

void CMorphwizardView::OnToolsSetParaNo()
{
	if (found_paradigms.empty())
	{
		ErrorMessage("Cannot std::set paradigm No to an empty found list");
		return;
	};
	POSITION pos = m_FoundList.GetFirstSelectedItemPosition();
	uint16_t ParadigmNo = 0;
	if (pos != 0)
	{
		size_t index = m_FoundList.GetNextSelectedItem(pos);
		assert(index < found_paradigms.size());
		ParadigmNo = found_paradigms[index]->second.m_FlexiaModelNo;
	}
	CString s;
	s.Format(_T("%i"), ParadigmNo);
	if (!InputBox(_T("Input paradigm No:"), s)) return;
	ParadigmNo = _ttoi(s);
	if (ParadigmNo >= GetWizard()->m_FlexiaModels.size())
	{
		ErrorMessage(Format("Paradigm no %i is not found", ParadigmNo));
		return;
	}
	CString Warn;
	Warn.Format(_T("The program ascribes all found words to paradigm %i . Continue?"), ParadigmNo);
	if (::MessageBox(0, Warn, _T("MorphWizard"), MB_OKCANCEL) != IDOK)
		return;

	const CFlexiaModel& new_par = GetWizard()->m_FlexiaModels[ParadigmNo];
	size_t ErrorsCount = 0;

	for (size_t i = 0; i < found_paradigms.size(); i++)
	{
		const CFlexiaModel& old_par = GetWizard()->m_FlexiaModels[found_paradigms[i]->second.m_FlexiaModelNo];

		if (found_paradigms[i]->second.m_FlexiaModelNo == ParadigmNo) continue;


		std::string new_flexia = new_par.get_first_flex();
		std::string lemma = GetWizard()->get_lemm_string(found_paradigms[i]);
		std::string lemma_suffix = lemma.substr(lemma.length() - new_flexia.length());

		if (new_flexia == lemma_suffix)
		{
			GetWizard()->change_prd_info(found_paradigms[i]->second, lemma, ParadigmNo, UnknownAccentModelNo, true);
		}
		else
		{
			ErrorsCount++;
		};
	};
	if (ErrorsCount > 0)
	{
		ErrorMessage(Format("This last operation cannot change lemma in %i cases.", ErrorsCount));
	};
	ShowFoundParadigms();
}

void CMorphwizardView::OnToolsSetAccentModelNo()
{
	if (found_paradigms.empty())
	{
		ErrorMessage("Cannot set accent model to an empty found list");
		return;
	};
	POSITION pos = m_FoundList.GetFirstSelectedItemPosition();
	uint16_t accentModeldNo = 0;
	if (pos != 0)
	{
		size_t index = m_FoundList.GetNextSelectedItem(pos);
		assert(index < found_paradigms.size());
		accentModeldNo = found_paradigms[index]->second.m_AccentModelNo;
	}
	CString s;
	s.Format(_T("%i"), accentModeldNo);
	if (!InputBox(_T("Input accent model No:"), s)) return;
	accentModeldNo = _ttoi(s);
	if (accentModeldNo >= GetWizard()->m_AccentModels.size())
	{
		ErrorMessage(Format("accent model no %i is not found", accentModeldNo));
		return;
	}
	std::set< std::pair<uint16_t, uint16_t> > flexia_model_to_accent_model;
	for (auto const& p : GetWizard()->m_LemmaToParadigm) {
		auto flex_and_accent = std::pair<uint16_t, uint16_t>(p.second.m_FlexiaModelNo, p.second.m_AccentModelNo);
		flexia_model_to_accent_model.insert(flex_and_accent);
	}

	for (auto const& p : found_paradigms) {
		auto flexia_model_no = p->second.m_FlexiaModelNo;
		auto p = std::pair<uint16_t, uint16_t>(flexia_model_no, accentModeldNo);
		if (flexia_model_to_accent_model.find(p) == flexia_model_to_accent_model.end()) {
			ErrorMessage(Format("This operation would set accent model no %i to a word with flexia model no %i, this association is unknown. The operation was canceled ", accentModeldNo, flexia_model_no));
			return;
		}
	}

	CString Warn;
	Warn.Format(_T("The program ascribes all found words to accent model %i . Continue?"), accentModeldNo);
	if (::MessageBox(0, Warn, _T("MorphWizard"), MB_OKCANCEL) != IDOK)
		return;


	for (auto& p : found_paradigms) {
		p->second.m_AccentModelNo = accentModeldNo;
	};
	ShowFoundParadigms();
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnFindParadigm()
{
	if (m_inFind)
		OnFind();
	else if (m_inPredict)
		OnPredict();
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnEditCopy()
{
	if (m_inFind)
		((CEdit*)GetDlgItem(IDC_FIND_WHAT))->Copy();
	if (m_inPredict)
		((CEdit*)GetDlgItem(IDC_PREDICT_WHAT))->Copy();
	if (m_inFilter)
		((CEdit*)GetDlgItem(IDC_FILTER))->Copy();
}

void CMorphwizardView::OnEditCut()
{
	if (m_inFind)
		((CEdit*)GetDlgItem(IDC_FIND_WHAT))->Cut();
	if (m_inPredict)
		((CEdit*)GetDlgItem(IDC_PREDICT_WHAT))->Cut();
	if (m_inFilter)
		((CEdit*)GetDlgItem(IDC_FILTER))->Cut();
}

void CMorphwizardView::OnEditPaste()
{
	if (m_inFind)
		((CEdit*)GetDlgItem(IDC_FIND_WHAT))->Paste();
	if (m_inPredict)
		((CEdit*)GetDlgItem(IDC_PREDICT_WHAT))->Paste();
	if (m_inFilter)
		((CEdit*)GetDlgItem(IDC_FILTER))->Paste();
}

void CMorphwizardView::OnEnKillfocusFilter()
{
	m_inFilter = false;
}

void CMorphwizardView::OnEnSetfocusFilter()
{

	m_inFilter = true;
}

void CMorphwizardView::OnHelpHelp()
{
	AfxGetApp()->WinHelp(IDH_LOADING);
}

void CMorphwizardView::OnToolsShowparadigmdifferences()
{
	CSLFDocument* pDocument = (CSLFDocument*)GetTemplate("MorphWizard.diff")->CreateNewDocument();
	pDocument->m_pParent = this;
	ASSERT_VALID(pDocument);
	pDocument->OpenDiffDialog();
}

void CMorphwizardView::OnToolsShowAccentDifferences()
{
	CSLFDocument* pDocument = (CSLFDocument*)GetTemplate("MorphWizard.accent_diff")->CreateNewDocument();
	pDocument->m_pParent = this;
	ASSERT_VALID(pDocument);
	pDocument->OpenDiffDialog();
}


//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsAccentWizard()
{
	CAccentWizardDlg dlg(*this);
	if (dlg.DoModal() == IDOK)
	{
		m_partialAccentedPara = dlg.m_allNonaccentedPara == FALSE;
		OpenExistingParadigm(FindNonAccentedPara(m_partialAccentedPara), true);
	}
}

//----------------------------------------------------------------------------
LRESULT CMorphwizardView::OnNextNonAccentedPara(WPARAM wp, LPARAM lp)
{
	OpenExistingParadigm(FindNonAccentedPara(m_partialAccentedPara), true);
	return TRUE;
}

//----------------------------------------------------------------------------
lemma_iterator_t CMorphwizardView::FindNonAccentedPara(bool partialAccentedPara)
{
	lemma_iterator_t it = GetWizard()->m_LemmaToParadigm.begin();
	for (;
		it != GetWizard()->m_LemmaToParadigm.end(); it++)
	{
		if (GetWizard()->HasUnknownAccents(it))
		{
			if (GetWizard()->IsPartialAccented(it) || !partialAccentedPara)
				return it;
		}
	}
	return it;
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}




void CMorphwizardView::OnBnClickedSetPrdComments()
{
	POSITION pos = m_FoundList.GetFirstSelectedItemPosition();

	if (pos == 0)
	{
		ErrorMessage("No item selected");
		echo("No item selected");
		return;
	};
	size_t index = m_FoundList.GetNextSelectedItem(pos);
	int ModelNo = found_paradigms[index]->second.m_FlexiaModelNo;
	CString Caption;
	Caption.Format(_T("Input comments to paradigm No:%i"), ModelNo);
	InputBox(Caption, FromInnerEncoding(GetWizard()->m_FlexiaModels[ModelNo].m_Comments));
}


int CMorphwizardView::PlaceSimpleControl(int id, int x, int y) {
	GetDlgItem(id)->SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	CRect r;
	GetDlgItem(id)->GetWindowRect(&r);
	x += r.Width() + m_ControlMargin;
	return x;
}

void CMorphwizardView::OnSize(UINT nType, int cx, int cy)
{
	if (!m_FindWhat || cx < 0 || cy < 0 || !m_FindWhat.m_hWnd) {
		CFormView::OnSize(nType, cx, cy);
		return;
	}
	int row_height = 40;
	int margin = m_ControlMargin;
	int center = (int)(cx / 2);
	int center_plus_margin = center + 2 * margin;
	int panel_width = (cx - 3 * margin) / 2;
	int panel_height = cy - 2 * margin - row_height;
	GetDlgItem(IDC_GROUP_BOX_EDIT)->SetWindowPos(&wndBottom, margin, margin, panel_width, panel_height, SWP_SHOWWINDOW);
	GetDlgItem(IDC_GROUP_BOX_ADD)->SetWindowPos(&wndBottom, center + margin, margin, panel_width, panel_height, SWP_SHOWWINDOW);

	// 1st line 
	int x = 2 * margin;
	int y = 3 * margin;
	x = PlaceSimpleControl(IDC_FIND, x, y);
	x = PlaceSimpleControl(IDC_FIND_WHAT, x, y);

	x = PlaceSimpleControl(IDC_PREDICT, center_plus_margin, y);
	x = PlaceSimpleControl(IDC_PREDICT_WHAT, x, y);
	x = PlaceSimpleControl(IDC_READONLY_IMAGE, cx - 50, y);

	// 2nd line
	x = 2 * margin;
	y += margin + row_height;
	x = PlaceSimpleControl(IDC_RFIND_LEM, x, y);
	x = PlaceSimpleControl(IDC_RFIND_GRM, x, y);
	x = PlaceSimpleControl(IDC_FIND_BY_USERNAME, x, y);
	x = PlaceSimpleControl(IDC_WORD_FORM, x, y);
	x = PlaceSimpleControl(IDC_FIND_BY_GRAMCODE, x, y);
	x = PlaceSimpleControl(IDC_FIND_BY_ACCENT_MODEL, x, y);
	x = PlaceSimpleControl(IDC_FIND_BY_PARADIGM_NO, x, y);


	x = PlaceSimpleControl(IDC_MIN_FREQ_STATIC, center_plus_margin, y);
	x = PlaceSimpleControl(IDC_MINIMAL_FREQUENCE, x, y);
	x = PlaceSimpleControl(IDC_SUF_STATIC, x, y);
	x = PlaceSimpleControl(IDC_SUF_EDIT, x, y);

	// 3th line
	x = 2 * margin;
	y += margin + row_height;
	int list_height = cy - 4 * row_height - 8 * margin;
	int list_width = (cx - 6 * margin) / 2;

	GetDlgItem(IDC_FOUND_LIST2)->SetWindowPos(&wndBottom, x, y, list_width, list_height, SWP_SHOWWINDOW);
	GetDlgItem(IDC_PREDICTED_LIST)->SetWindowPos(&wndBottom, center_plus_margin, y, list_width, list_height, SWP_SHOWWINDOW);

	// 4th line 
	x = 2 * margin;
	y += margin + list_height;

	x = PlaceSimpleControl(IDC_REMOVE, x, y);
	x = PlaceSimpleControl(IDC_EDIT, x, y);
	x = PlaceSimpleControl(IDC_SET_PRD_COMMENTS, x, y);
	x = PlaceSimpleControl(IDC_FILTER_STATIC, x, y);
	x = PlaceSimpleControl(IDC_FILTER, x, y);


	x = PlaceSimpleControl(IDC_ADD, center_plus_margin, y);
	x = PlaceSimpleControl(IDC_ONLY_MAIN_PART_OF_SPEECHES, x, y);

	// 5th line
	x = margin;
	y += margin + row_height;
	GetDlgItem(IDC_STATUS)->SetWindowPos(&wndBottom, x, y, cx - 2 * margin, row_height, SWP_SHOWWINDOW);

	if (!GetWizard()->m_ReadOnly)
	{
		GetDescendantWindow(IDC_READONLY_IMAGE)->ShowWindow(SW_HIDE);
	}

	CFormView::OnSize(nType, cx, cy);
}
