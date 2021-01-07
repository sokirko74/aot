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

//----------------------------------------------------------------------------
CDocTemplate* GetTemplate( CString Name )
{
	// получение template документа статьи
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* tmpl;
	CString S;
	do {
		tmpl = AfxGetApp()->GetNextDocTemplate (pos);
		tmpl->GetDocString (S, CDocTemplate::regFileTypeId);
	}
	while ((tmpl != 0) &&  (S != Name));

	ASSERT (tmpl);

	return tmpl;
}


//----------------------------------------------------------------------------
// CMorphwizardView
//----------------------------------------------------------------------------

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
	ON_BN_CLICKED(IDC_READONLY_IMAGE, OnReadonlyImage)
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
	ON_COMMAND(ID_TOOLS_SELECT_BY_FILE, OnToolsSelectByFile)
	ON_COMMAND(ID_TOOLS_SHOW_PARA_DIFF, OnToolsShowparadigmdifferences)
	ON_COMMAND(ID_TOOLS_ACCENTWIZARD, OnToolsAccentWizard)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EXPORT, OnUpdateToolsExport)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_IMPORT, OnUpdateToolsImport)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PACK_MRD, OnUpdateToolsPackMrd)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SET_PARA_NO, OnUpdateToolsSetParaNo)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ACCENTWIZARD, OnUpdateToolsAccentWizard)
	ON_MESSAGE(WM_NEXT_NONACCENTED_PARA, OnNextNonAccentedPara)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SAVE_LIST_FILE, OnUpdateToolsSaveListFile)
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


// CMorphwizardView drawing

void CMorphwizardView::OnDraw(CDC* /*pDC*/)
{
	CMorphwizardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}


// CMorphwizardView diagnostics

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


const size_t PredictGrammemsColumn = 0;
const size_t PredictTypeDictColumn = 1;
const size_t PredictLemmaColumn = 2;
const size_t PredictFreqColumnNo = 3;
const size_t PredictIndexColumnNo = 4;
const size_t PredictParadigmColumnNo  = 5;
const size_t PredictLemmaPrefixColumnNo  = 6;


const size_t FindParadigmColumnNo  = 0;
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
	GetParent()->SetWindowPos(NULL, (GlobalX-DlgWidth)/2, (GlobalY-DlgHeight)/2-60, DlgWidth, DlgHeight, SWP_SHOWWINDOW|SWP_NOZORDER);
	GetParent()->ShowWindow(SW_MAXIMIZE); // Nick [12/Dec/2003]
	
	m_PredictedList.InsertColumn(PredictGrammemsColumn,_T("Grammems"),LVCFMT_LEFT, 90);
	m_PredictedList.InsertColumn(PredictTypeDictColumn,_T("Type"),LVCFMT_LEFT, 35);
	m_PredictedList.InsertColumn(PredictLemmaColumn,_T("Source lemma"),LVCFMT_LEFT, 90);
	m_PredictedList.InsertColumn(PredictFreqColumnNo,_T("Freq"),LVCFMT_LEFT, 50);
	m_PredictedList.InsertColumn(PredictIndexColumnNo,_T("Innernumber"), LVCFMT_LEFT, 0);
	m_PredictedList.InsertColumn(PredictParadigmColumnNo, _T("ParadigmNo"), LVCFMT_LEFT, 50);
	m_PredictedList.InsertColumn(PredictLemmaPrefixColumnNo, _T("Prefix"), LVCFMT_LEFT, 50);
	m_PredictedList.SetExtendedStyle(LVS_EX_FULLROWSELECT);  

	m_FoundList.InsertColumn(FindParadigmColumnNo,_T("ParadigmNo"),LVCFMT_LEFT, 50);
	m_FoundList.InsertColumn(FindLemmaColumn, _T("Lemma"),LVCFMT_LEFT, 130);
	m_FoundList.InsertColumn(FindGrammemsColumn, _T("Grammems"),LVCFMT_LEFT, 210);
	m_FoundList.InsertColumn(FindTypeGrammemsColumn, _T("Type"),LVCFMT_LEFT, 210);
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

	CheckRadioButton(IDC_RFIND_LEM,IDC_WORD_FORM,IDC_RFIND_LEM);
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
	return (HCURSOR) m_hIcon;
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
	if( flt_str.empty() ) return;
	std::string common_ancode;
	if (isdigit ((BYTE)flt_str[0]) )
	{
		if (strspn(flt_str.c_str(), "0123456789") != flt_str.length())
		{
			ErrorMessage ("Bad format for paradigm No in filter");
			return;
		};
		size_t FlexiaModel = atoi(flt_str.c_str() );
		if( FlexiaModel >= GetWizard()->m_FlexiaModels.size() )
		{
			ErrorMessage ("Paradigm No in filter is too high ");
			return;
		};
		std::vector<lemma_iterator_t> filter_paradigms;
		for( int i=0; i<found_paradigms.size(); i++ )
		{
			if (found_paradigms[i]->second.m_FlexiaModelNo != FlexiaModel) continue;
			filter_paradigms.push_back(found_paradigms[i]);
		}
		found_paradigms = filter_paradigms;

	}
	else
	{
        if (!GetWizard()->Filter(flt_str, found_paradigms)) {
            AfxMessageBox(IDS_BADFSTR,MB_ICONERROR|MB_OK);
        }
	};

};

void CMorphwizardView::ShowFoundParadigms() 
{
	m_FoundList.DeleteAllItems();
	
	for( int i=0; i<found_paradigms.size(); i++ )
	{
		int FlexiaModelNo = found_paradigms[i]->second.m_FlexiaModelNo;
		CString S;
		S.Format(_T("%i (%i)"), FlexiaModelNo,found_paradigms[i]->second.m_AccentModelNo);
		m_FoundList.InsertItem(i, S );

		std::string Lemma = GetWizard()->get_lemm_string_with_accents(found_paradigms[i]);
		std::string PrefixSet = GetWizard()->get_prefix_set( found_paradigms[i] );
		if (!PrefixSet.empty())
			Lemma = PrefixSet+'|'+Lemma;
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
		if (find_what == "") return;
		ChangeHistory(find_what);
		UpdateData(FALSE);
		std::string find_what_rml = ToInnerEncoding(find_what);

		CWizardProgressMeter meter(*GetWizard());

		if(GetCheckedRadioButton(IDC_RFIND_LEM,IDC_WORD_FORM)==IDC_RFIND_LEM)
		{
			std::string s = find_what_rml;
			RmlMakeUpper(s, GetWizard()->m_Language);
			Trim(s);
			char ch = s[0];
			if( '0'<=ch && ch<='9' )
			{
				int prdno = atoi(s.c_str());
				GetWizard()->find_lemm_by_prdno(prdno,found_paradigms);
			}
			else
			{
				if (s.substr(0,7) == "ACCENT=")
				{
					int accent_model_no = atoi(s.c_str()+7);
					GetWizard()->find_lemm_by_accent_model(accent_model_no, found_paradigms);
				}
				else
					GetWizard()->find_lemm(s.c_str(), false, found_paradigms);
			};
		}
	//
		if(GetCheckedRadioButton(IDC_RFIND_LEM,IDC_WORD_FORM)==IDC_RFIND_GRM)
			GetWizard()->find_lemm_by_grammem(find_what_rml, found_paradigms);

		if(GetCheckedRadioButton(IDC_RFIND_LEM,IDC_WORD_FORM)==IDC_WORD_FORM)
		{
			std::string s = find_what_rml;
			// нельзя применять  RmlMakeUpper из-за "*"
			if (GetWizard()->IsGerman())
				GerMakeUpper(s);
			else
				EngRusMakeUpper(s);
			GetWizard()->find_wordforms(s.c_str(), found_paradigms);
		};

		//  findinsg by gramcode
		if(GetCheckedRadioButton(IDC_RFIND_LEM,IDC_FIND_BY_GRAMCODE)==IDC_FIND_BY_GRAMCODE)
		{
			GetWizard()->find_ancodes(find_what_rml.c_str(), found_paradigms);
		};
		if(GetCheckedRadioButton(IDC_RFIND_LEM,IDC_FIND_BY_USERNAME)==IDC_FIND_BY_USERNAME)
		{
			GetWizard()->find_lemm_by_user(find_what_rml.c_str(), found_paradigms);
		};
		

	// proceed filter std::string
		FilterFoundParadigms();
		ShowFoundParadigms();
	}
	catch (CExpc C)
	{
		ErrorMessage(C.m_strCause);
	}
}



void CMorphwizardView::OnRemove() 
{
	try 
	{
		POSITION pos = m_FoundList.GetFirstSelectedItemPosition();		
		if(pos == 0)
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
		ErrorMessage ("An exception occurred!");
	};
}


class IsLessForPredict  
{
	
	CMorphwizardView* m_pView;
public:
	IsLessForPredict(CMorphwizardView* pView)
	{
		m_pView = pView;
	}  

	bool  operator()(const size_t& _X1, const size_t& _X2) const
	{
		const MorphoWizard* W = m_pView->GetWizard();
		const CPredictSuffix& S1 = *W->m_CurrentPredictedParadigms[_X1];
		const CPredictSuffix& S2 = *W->m_CurrentPredictedParadigms[_X2];
		switch (m_pView->m_SortPredictColumn)
		{
			case PredictGrammemsColumn: 
				{
					std::string s1 = W->get_pos_string_and_grammems(S1.m_SourceLemmaAncode);
					std::string s2 = W->get_pos_string_and_grammems(S2.m_SourceLemmaAncode);
					if ( s1 == s2) return S1.m_Frequence > S2.m_Frequence;
					else
						return s1 < s2;
				};
			case PredictTypeDictColumn: 
				{
					std::string s1 = W->get_grammem_string(S1.m_SourceCommonAncode);
					std::string s2 = W->get_grammem_string(S2.m_SourceCommonAncode);
					if ( s1 == s2) return S1.m_Frequence > S2.m_Frequence;
					else
						return s1 < s2;
				};
			case PredictLemmaColumn: 
				{
					std::string s1 = S1.m_SourceLemma;
					std::string s2 = S2.m_SourceLemma;
					if ( s1 == s2) return S1.m_Frequence > S2.m_Frequence;
					else
						return s1 < s2;
				};
			case PredictFreqColumnNo: return S1.m_Frequence > S2.m_Frequence;
			case PredictParadigmColumnNo: return S1.m_FlexiaModelNo < S2.m_FlexiaModelNo;
			case PredictLemmaPrefixColumnNo: 
				{
					std::string s1 = S1.m_PrefixSetStr;
					std::string s2 = S2.m_PrefixSetStr;
					if ( s1 == s2) return S1.m_Frequence > S2.m_Frequence;
					else
						return s1 < s2;
				};
			default : return _X1 < _X2;
		};
	};

};

std::string CMorphwizardView::ToInnerEncoding(CString strText) const 
{
	std::wstring wstr((LPCTSTR)strText);
	return GetDocument()->m_Wizard.ToRMLEncoding(wstr);
}

CString CMorphwizardView::FromInnerEncoding(std::string s) const
{
	return GetDocument()->m_Wizard.FromRMLEncoding(s).c_str();
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
			predict_what = predict_what.Mid(i+1);
			Lemma = ToInnerEncoding(prefix+predict_what);
		};

		GetWizard()->find_lemm(Lemma.c_str(), false, curr_found_paradigms);
		if (!curr_found_paradigms.empty())
			AfxMessageBox (_T("The dictionary contains already this word!"));


		GetWizard()->predict_lemm(ToInnerEncoding(predict_what), m_PredictSuffLength, m_MinimalFrequence, m_bOnlyMainPartOfSpeeches==TRUE);
		if (GetWizard()->m_CurrentPredictedParadigms.empty())
		{
			ErrorMessage("No variants found");
			return;
		};
		std::vector<size_t> Index;
		for(size_t i=0; i < GetWizard()->m_CurrentPredictedParadigms.size(); i++)
			Index.push_back(i);
		std::sort (Index.begin(), Index.end(), IsLessForPredict(this));
		for(size_t i = 0; i < Index.size(); i++)
		{
			size_t  ind = Index[i];
			const CPredictSuffix& S = *GetWizard()->m_CurrentPredictedParadigms[ind];
			const CFlexiaModel& P = GetWizard()->m_FlexiaModels[S.m_FlexiaModelNo];

			m_PredictedList.InsertItem(i, 
				FromInnerEncoding(GetWizard()->get_pos_string_and_grammems(S.m_SourceLemmaAncode)) );
			m_PredictedList.SetItemText(i, 
						PredictTypeDictColumn, 
						FromInnerEncoding(GetWizard()->get_grammem_string(S.m_SourceCommonAncode)));
			m_PredictedList.SetItemText(i, PredictLemmaColumn, FromInnerEncoding(S.m_SourceLemma));
			m_PredictedList.SetItemText(i, PredictFreqColumnNo, IntToStr(S.m_Frequence));
			m_PredictedList.SetItemText(i, PredictIndexColumnNo, IntToStr(ind));
			m_PredictedList.SetItemText(i, PredictParadigmColumnNo, IntToStr(S.m_FlexiaModelNo));
			m_PredictedList.SetItemText(i, PredictLemmaPrefixColumnNo,  FromInnerEncoding(S.m_PrefixSetStr));
		}
		OnFind();
		m_PredictWhat.SetFocus();
	}
	catch (...)
	{
	ErrorMessage("wizard", "an exception occurred!");

	};
	
}

CSLFDocument* CMorphwizardView::NewSLFDocument ()
{
	CSLFDocument* pDocument = (CSLFDocument*)GetTemplate("MorphWizard.slf")->CreateNewDocument();
	pDocument->m_pParent = this;
	ASSERT_VALID(pDocument);
	return pDocument;
};

void CMorphwizardView::OnAdd() 
{
	POSITION pos = m_PredictedList.GetFirstSelectedItemPosition();		

	try{
		CSLFDocument* pDocument = NewSLFDocument();
				
		CString predict_what;
		m_PredictWhat.GetWindowText(predict_what);
		if (pos == NULL)
		{
			predict_what += " ";
			pDocument->m_ParadigmText = predict_what;
			pDocument->m_CommonGrammems = "";
		}else 
		{
			size_t ind = m_PredictedList.GetNextSelectedItem(pos);
			CString S = m_PredictedList.GetItemText(ind, PredictIndexColumnNo);
			std::string common_grammems;
			pDocument->m_ParadigmText = FromInnerEncoding(GetWizard()->create_slf_from_predicted(_wtoi(S), common_grammems, 50));
			pDocument->m_CommonGrammems = FromInnerEncoding(common_grammems);
			int i = predict_what.Find(_T("|"));
			if (i != -1)
				pDocument->m_Prefixes =  predict_what.Mid(0, i);
		}


		pDocument->OpenParadigm();

	}catch(CExpc& e){
		echo(e.m_strCause.c_str());
	}catch(...){
		echo("error");
	}
}

//----------------------------------------------------------------------------
bool CMorphwizardView::OpenExistingParadigm( lemma_iterator_t it, bool bRunAccentWizard )
{
	try
	{
		if( it==GetWizard()->m_LemmaToParadigm.end() )
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
	catch(CExpc& e)
	{
		echo(e.m_strCause.c_str());
	}
	catch(...)
	{
		echo("error");
	}
	return false;
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnEdit() 
{
	POSITION pos = m_FoundList.GetFirstSelectedItemPosition();		
	if(pos == 0)
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
	FILE * fp = fopen (utf16_to_utf8((const TCHAR*)D.GetPathName()).c_str(), "wb");

	for( int i=0; i<found_paradigms.size(); i++ )
	{
		std::string lemma = GetWizard()->get_lemm_string_with_accents(found_paradigms[i]);
		std::string PrefixSet = GetWizard()->get_prefix_set( found_paradigms[i] );
		if (!PrefixSet.empty())
			lemma = PrefixSet+'|'+lemma;
		RmlMakeLower(lemma, GetWizard()->m_Language);
        std::string common_grams = "* "+GetWizard()->get_grammem_string(found_paradigms[i]->second.GetCommonAncodeIfCan());

		std::string s = lemma 
			+ ";"
			+ common_grams
			+ ";"
			+ GetWizard()->get_pos_string(found_paradigms[i]) 
			+ " " 
			+ GetWizard()->get_grammem_string(found_paradigms[i]);
		fprintf (fp, "%s\n", s.c_str());
			
	}
    fclose (fp);
}

//----------------------------------------------------------------------------
static BOOL remove_ancode(const std::string &str,const std::string &ancode,std::string &res)
{
	res = str;
	int pos1 = str.find(ancode);
	if( pos1==std::string::npos )
		return( FALSE );
	int pos2 = pos1+ancode.size();
	int grp1 = str.rfind("*",pos1);
	if( grp1==std::string::npos )
		return( FALSE );
	int grp2 = str.find("%",pos2);
	if( grp2==std::string::npos )
		grp2 = str.size();
	if( grp1+1<pos1 || pos2<grp2 )
	{
			res = str.substr(pos1,pos2-pos1);
			res = str;
		res.erase(pos1,pos2-pos1);
		return( TRUE );
	}
	int grp0 = str.rfind("%",grp1);
	if( grp0==std::string::npos )
		return( FALSE );
		res = str.substr(grp0,grp2-grp0);
		res = str;
	res.erase(grp0,grp2-grp0);
	return( TRUE );
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
		FILE * fp = fopen (utf16_to_utf8((const TCHAR*)D.GetPathName()).c_str(), "wb");
		if (!fp) 
		{
			AfxMessageBox (_T("Cannot open file"));
			return;
		};
		
		CProgressMeterRML meter;
		meter.SetMaxPos(found_paradigms.size(),100);
		CString info = _T("Exporting ") + D.GetPathName() + _T("..."); 
		meter.SetInfo(info);

		GetWizard()->set_to_delete_false();
		for  (long i=0;  i <found_paradigms.size(); i++) 
		{
			CDumpParadigm P;
			P.m_SlfStr = GetWizard()->get_slf_string(found_paradigms[i], P.m_TypeGrammemsStr, P.m_PrefixesStr, 50);
			
			if (found_paradigms[i]->second.m_PrefixSetNo != UnknownPrefixSetNo)
				P.m_PrefixesStr = GetWizard()->get_prefix_set(found_paradigms[i]);

			if ( !found_paradigms[i]->second.GetCommonAncodeIfCan().empty() )
				P.m_TypeGrammemsStr =  GetWizard()->get_grammem_string(found_paradigms[i]->second.GetCommonAncodeIfCan());

			if (found_paradigms[i]->second.m_SessionNo != UnknownSessionNo)
				P.m_Session =  GetWizard()->get_session(found_paradigms[i]->second.m_SessionNo);

			P.SaveToFile(fp);

			found_paradigms[i]->second.m_bToDelete = true;

			meter.AddPos();
		}
		fclose (fp);

		if (::MessageBox (0,_T("Delete all exported paradigms?"), _T("MorphWizard"), MB_YESNO   ) == IDYES)
		  GetWizard()->delete_checked_lemms();
	}
   catch (...)
   {
	   			AfxMessageBox (_T("some error has occured"));
   };

    
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsImport() 
{
	int line_no = 0;
	CString PathName;

try {
   	CFileDialog D(TRUE, _T("slf"), _T("paradigms.slf"),OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	TCHAR file_buffer[10000];
	file_buffer[0] = 0;
	D.m_ofn.lpstrFile = file_buffer;
	D.m_ofn.nMaxFile = 10000;
	if (D.DoModal() != IDOK) return;
	POSITION pos = D.GetStartPosition();
	if (pos == 0) return;
	bool bTestMode = ::MessageBox (0, _T("Test it or import?"), _T("MorphWizard"), MB_YESNO   ) == IDYES;
	
	GetWizard()->m_bFullTrace = false;
	PathName = D.GetNextPathName(pos);
	while  (true) // all selected files 
	{

		FILE * fp = fopen (utf16_to_utf8((const TCHAR*)PathName).c_str(), "r");
		if (!fp) 
		{
			AfxMessageBox (_T("Cannot open file"));
			return;
		};
				
		GetWizard()->log (std::string("import file ") + ToInnerEncoding(PathName));
				
		CProgressMeterRML meter;
		meter.SetFileMaxPos(fp);
		CString info = (bTestMode?"Test":"Import");
		info+= "ing file " + PathName + "...";
		meter.SetInfo(info);
				
		int ParadigmCount = 0;
		std::string Errors;
		line_no = 0;
		bool bError;
		CDumpParadigm P;
		while  (GetWizard()->ReadNextParadigmFromFile(fp, P, line_no, bError, Errors)) 
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
						GetWizard()->slf_to_mrd (P.m_SlfStr, lemm, Dummy1, AccentModel, Dummy, line_no_err);
					}
					else
					{
						uint16_t SessionNo = GetWizard()->RegisterSession(P.m_Session);
						GetWizard()->add_lemma(P.m_SlfStr, P.m_TypeGrammemsStr, P.m_PrefixesStr, line_no_err, SessionNo);
					};

					meter.SetFilePos();		  
				}
				catch (CExpc C)
				{
					Errors += Format("%s (%s:%i) \n", C.m_strCause.c_str(), (const TCHAR*)PathName, P.m_FirstSlfLineNo+line_no_err);
				}
				catch  (...) 
				{
					Errors += Format("error at %s:%i \n", (const TCHAR*)PathName, P.m_FirstSlfLineNo+line_no_err);
				};
					
			}
		}
		fclose (fp);
		if (!Errors.empty())
		{
			try {
				std::string ErrorFile = MakeFName(ToInnerEncoding(PathName).c_str(), "err");
				FILE * fp = fopen (ErrorFile.c_str(), "w");
				fprintf (fp, "%s",Errors.c_str());
				fclose(fp);
				ErrorMessage(Format("Errors were written to file %s",  ErrorFile.c_str()));
			} 
			catch (...){
				AfxMessageBox (_T("Cannot write errors to paradigms.err "));
			}
		}
		else
		{
			ErrorMessage(Format("Successfully %s %i paradigms from \"%s\"" , bTestMode? "tested":"imported", ParadigmCount, PathName), "Confirmation");
		};
		if (pos == 0) break;
		PathName = D.GetNextPathName(pos);
	} // all selected files

}
catch (...)
{
	ErrorMessage (Format("some error has occurred (file=%s, line=%i", PathName, line_no));
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
	FILE * fp = fopen (utf16_to_utf8((const TCHAR*)D.GetPathName()).c_str(), "rb");
	if (!fp) 
	{
		AfxMessageBox (_T("Cannot open file"));
		return;
	};
	char buf[1000];
	std::string strNotFound;
	int ParadigmCount = 0;
	found_paradigms.clear();
	while  (fgets(buf, 1000, fp)) 
	{
		std::string Line = convert_from_utf8(buf, GetWizard()->m_Language);
		Trim(Line);
		if (Line.empty()) continue;
		StringTokenizer tok (Line.c_str(), ";");
		if (!tok())
		{
			std::string mess = std::string("cannot get lemma from ") + buf + std::string("; The format should be <Lemma>;<TypeGrammems>;<MorphPattern>");
			AfxMessageBox (FromInnerEncoding(mess));
			break;
		};
		std::string Lemma = tok.val();
		Trim(Lemma);


		if (!tok())
		{
			std::string mess = std::string("cannot get type grammem ") + buf + std::string("; The format should be <Lemma> <PartofSpeech> <Grammems>");
			AfxMessageBox (FromInnerEncoding(mess));
			break;
		};
		std::string TypeAncode;
		{
			std::string grams = tok.val();
			Trim(grams);
			if (grams != "*")
				if (!GetWizard()->slf2ancode(grams, TypeAncode )) 
				{
					std::string mess = std::string("cannot process type grammems ") + grams;
					AfxMessageBox (FromInnerEncoding(mess));
					break;
				};
		};


		if (!tok())
		{
			std::string mess = std::string("cannot get morphological pattern ") + buf + std::string("; The format should be <Lemma>;<TypeGrammems>;<MorphPattern>");
			AfxMessageBox (FromInnerEncoding(mess));
			break;
		};
		std::string FirstCode;
		{
			std::string PosStr = tok.val();
			Trim(PosStr);
			if (!GetWizard()->slf2ancode(PosStr,FirstCode )) 
			{
				std::string mess = std::string("cannot process morph. pattern ") + PosStr;
				AfxMessageBox (FromInnerEncoding(mess));
				break;
			};
		};

		std::vector<lemma_iterator_t> curr_found_paradigms;
		RmlMakeUpper(Lemma, GetWizard()->m_Language);
		bool bFound = false;
		GetWizard()->find_lemm(Lemma.c_str(), true, curr_found_paradigms);
		for(size_t i=0; i<curr_found_paradigms.size(); i++ )
		{
			std::string str_pos = GetWizard()->m_FlexiaModels[curr_found_paradigms[i]->second.m_FlexiaModelNo].get_first_code();;

			if (curr_found_paradigms[i]->second.GetCommonAncodeIfCan() == TypeAncode)
				if( FirstCode == str_pos )
					if (std::find(found_paradigms.begin(), found_paradigms.end(), curr_found_paradigms[i]) == found_paradigms.end())
					{
						found_paradigms.push_back(curr_found_paradigms[i]);
						bFound = true;
					};
		}
		if (!bFound)
			strNotFound +=  Format("Not found: %s\r\n", Line.c_str());


	};
	fclose(fp);

	FilterFoundParadigms();
	ShowFoundParadigms();	
	if (!strNotFound.empty())
		echo(strNotFound.c_str());
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnReadonlyImage() 
{
    auto FileName = GetWizard()->get_lock_file_name();
	if (access(FileName.c_str(), 0) != -1)
	{
	    char s [1000];
		CString Mess;
        FILE* fp =  fopen (FileName.c_str(), "r");
		if (!fp) return;
		while (fgets(s, 1000, fp))
			Mess += s;
		fclose(fp);
		::MessageBox (0, Mess, _T("Lock Information"), MB_OK);
	}
}


/////////////////////////////////////////////////////////////////////////////
bool IsLessByParadigmNo (const lemma_iterator_t& _X1, const lemma_iterator_t& _X2)  
{
	return  _X1->second.m_FlexiaModelNo < _X2->second.m_FlexiaModelNo;

}

bool IsLessByLemma (const lemma_iterator_t& _X1, const lemma_iterator_t& _X2)  
{
	return  _X1->first < _X2->first;

}

//----------------------------------------------------------------------------
void CMorphwizardView::OnLvnColumnclickFoundList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int ColumnNo = pNMLV->iSubItem;
	if (ColumnNo == FindParadigmColumnNo)
	{
		std::sort (found_paradigms.begin(), found_paradigms.end(), IsLessByParadigmNo);
		ShowFoundParadigms();
	}
	else
	if (FindLemmaColumn == ColumnNo)
	{
		std::sort (found_paradigms.begin(), found_paradigms.end(), IsLessByLemma);
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
	AfxMessageBox (_T("Packing is finished!"));
}

BOOL CMorphwizardView::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	AfxGetApp()->WinHelp(IDH_MORPH_WIZARD_DLG);
	return TRUE;
}

void CMorphwizardView::OnNMDblclkFoundList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnEdit();	
	*pResult = 0;
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsSetParaNo()
{
	if (found_paradigms.empty()) 
	{
		ErrorMessage("Cannot std::set paradigm No to an empty found list");
		return;
	};
	POSITION pos = m_FoundList.GetFirstSelectedItemPosition();		
	uint16_t ParadigmNo = 0;
	if(pos != 0)
	{
		size_t index = m_FoundList.GetNextSelectedItem(pos);
		assert(index < found_paradigms.size());
		ParadigmNo = found_paradigms[index]->second.m_FlexiaModelNo;
	}
	CString s;
	s.Format(_T("%i"), ParadigmNo);
	if ( !InputBox(_T("Input paradigm No:"), s) ) return;
	ParadigmNo = _ttoi(s);
	if (ParadigmNo >= GetWizard()->m_FlexiaModels.size())
	{
			ErrorMessage(Format("Paradigm no %i is not found",ParadigmNo));
			return;
	}
	CString Warn;
	Warn.Format(_T("The program ascribes all found words to paradigm %i . Continue?"), ParadigmNo);
	if  (::MessageBox (0, Warn, _T("MorphWizard"), MB_OKCANCEL   ) != IDOK )
		return;

	const CFlexiaModel &new_par = GetWizard()->m_FlexiaModels[ParadigmNo];
	size_t ErrorsCount = 0;

	for (size_t i=0; i<found_paradigms.size(); i++)
	{
		const CFlexiaModel &old_par = GetWizard()->m_FlexiaModels[found_paradigms[i]->second.m_FlexiaModelNo];

		if (found_paradigms[i]->second.m_FlexiaModelNo == ParadigmNo) continue;

		
		std::string new_flexia = new_par.get_first_flex();
		std::string lemma = GetWizard()->get_lemm_string(found_paradigms[i]);
		std::string lemma_suffix = lemma.substr(lemma.length() - new_flexia.length());

		if (new_flexia == lemma_suffix)
		{
			GetWizard()->change_prd_info(found_paradigms[i]->second, lemma, ParadigmNo,UnknownAccentModelNo,true);
		}
		else
		{
			ErrorsCount++;
		};
	};
	if (ErrorsCount > 0)
	{
		ErrorMessage (Format("This last operation cannot change lemma in %i cases.", ErrorsCount));
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
			((CEdit*)GetDlgItem(IDC_FILTER ))->Copy();
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
			((CEdit*)GetDlgItem(IDC_FILTER ))->Paste();
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


void CMorphwizardView::OnUpdateToolsExport(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(!GetWizard()->m_ReadOnly);
}

void CMorphwizardView::OnUpdateToolsImport(CCmdUI *pCmdUI)
{
	// pCmdUI->Enable(!GetWizard()->m_ReadOnly);
}

void CMorphwizardView::OnUpdateToolsPackMrd(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(!GetWizard()->m_ReadOnly);
}

void CMorphwizardView::OnUpdateToolsSetParaNo(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(!GetWizard()->m_ReadOnly);
}

void CMorphwizardView::OnUpdateToolsSaveListFile(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(m_FoundList.GetItemCount()>0);
}

void CMorphwizardView::OnUpdateToolsAccentWizard(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(!GetWizard()->m_ReadOnly);
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsAccentWizard()
{
	CAccentWizardDlg dlg(*this);
	if( dlg.DoModal()==IDOK )
	{
		m_partialAccentedPara = dlg.m_allNonaccentedPara==FALSE;
		OpenExistingParadigm(FindNonAccentedPara(m_partialAccentedPara),true);
	}
}

//----------------------------------------------------------------------------
LRESULT CMorphwizardView::OnNextNonAccentedPara( WPARAM wp, LPARAM lp )
{
	OpenExistingParadigm(FindNonAccentedPara(m_partialAccentedPara),true);
	return TRUE;	
}

//----------------------------------------------------------------------------
lemma_iterator_t CMorphwizardView::FindNonAccentedPara( bool partialAccentedPara )
{
	lemma_iterator_t it=GetWizard()->m_LemmaToParadigm.begin();
	for( ; 
		it!=GetWizard()->m_LemmaToParadigm.end(); it++ )
	{
		if( GetWizard()->HasUnknownAccents(it) ) 
		{
			if( GetWizard()->IsPartialAccented(it) || !partialAccentedPara ) 
				return it;
		}
	}
	return it;		
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	SetInputLanguage(GetWizard()->m_Language);
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}




void CMorphwizardView::OnBnClickedSetPrdComments()
{
	POSITION pos = m_FoundList.GetFirstSelectedItemPosition();		

	if(pos == 0)
	{
		ErrorMessage ("No item selected");
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
	GetDlgItem(IDC_GROUP_BOX_EDIT)->SetWindowPos(&wndBottom, margin,         margin, panel_width, panel_height, SWP_SHOWWINDOW);
	GetDlgItem(IDC_GROUP_BOX_ADD)->SetWindowPos(&wndBottom, center + margin, margin, panel_width, panel_height, SWP_SHOWWINDOW);

	// 1st line 
	int x = 2 * margin;
	int y = 3 * margin;
	x = PlaceSimpleControl(IDC_FIND, x, y);
	x = PlaceSimpleControl(IDC_FIND_WHAT, x, y);

	x = PlaceSimpleControl(IDC_PREDICT, center_plus_margin, y);
	x = PlaceSimpleControl(IDC_PREDICT_WHAT, x, y);
	x = PlaceSimpleControl(IDC_READONLY_IMAGE, cx-50, y);
	
	// 2nd line
	x = 2 * margin;
	y +=  margin + row_height;
	x = PlaceSimpleControl(IDC_RFIND_LEM, x, y);
	x = PlaceSimpleControl(IDC_RFIND_GRM, x, y);
	x = PlaceSimpleControl(IDC_FIND_BY_USERNAME, x, y);
	x = PlaceSimpleControl(IDC_WORD_FORM, x, y);
	x = PlaceSimpleControl(IDC_FIND_BY_GRAMCODE, x, y);

	x = PlaceSimpleControl(IDC_MIN_FREQ_STATIC, center_plus_margin, y);
	x = PlaceSimpleControl(IDC_MINIMAL_FREQUENCE, x, y);
	x = PlaceSimpleControl(IDC_SUF_STATIC, x, y);
	x = PlaceSimpleControl(IDC_SUF_EDIT, x, y);

	// 3th line
	x = 2 * margin;
	y +=  margin + row_height;
	int list_height = cy - 4 * row_height - 8 * margin; 
	int list_width = (cx - 6 * margin) / 2;
	
	GetDlgItem(IDC_FOUND_LIST2)->SetWindowPos   (&wndBottom, x,                   y, list_width, list_height, SWP_SHOWWINDOW);
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

	CFormView::OnSize(nType, cx, cy);
}
