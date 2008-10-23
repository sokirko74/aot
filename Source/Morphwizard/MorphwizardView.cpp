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

#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#endif

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

IMPLEMENT_DYNCREATE(CMorphwizardView, CSizeFormView)

BEGIN_MESSAGE_MAP(CMorphwizardView, CSizeFormView)
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
END_MESSAGE_MAP()

void CMorphwizardView::DoDataExchange(CDataExchange* pDX)
{
	CView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMorphwizardView)
	DDX_Control(pDX, IDC_PREDICTED_LIST, m_PredictedList);
	DDX_Control(pDX, IDC_PREDICT_WHAT, m_PredictWhat);
	DDX_Control(pDX, IDC_FIND_WHAT, m_FindWhat);
	DDX_Control(pDX, IDC_STATUS, m_StatusCtrl);
	DDX_Text(pDX, IDC_STATUS, m_Status);

	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_MINIMAL_FREQUENCE, m_MinimalFrequence);
	DDX_Check(pDX, IDC_ONLY_MAIN_PART_OF_SPEECHES, m_bOnlyMainPartOfSpeeches);
	DDX_Control(pDX, IDC_FOUND_LIST2, m_FoundList);


	DDX_Text(pDX, IDC_EDIT1, m_PredictSuffLength);
	DDV_MinMaxInt(pDX, m_PredictSuffLength, 2, 5);

	//  init history
	m_FindWhat.ResetContent();
	list<string>::iterator pos = m_LastQueries.begin();
	while (pos != m_LastQueries.end())
	{
			m_FindWhat.AddString(pos->c_str());
			pos++;
	}
	m_FindWhat.SetCurSel(0);


}

//----------------------------------------------------------------------------
CMorphwizardView::CMorphwizardView() :
 CSizeFormView(CMorphwizardView::IDD)
	 , m_PredictSuffLength(2)
{
	 m_SortPredictColumn = -1;
	 LoadHistory();
}

CMorphwizardView::~CMorphwizardView()
{
	SaveHistory();
}

MorphoWizard* CMorphwizardView::GetWizard() 
{
	return  &(GetDocument()->m_Wizard);
};

BOOL CMorphwizardView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CSizeFormView::PreCreateWindow(cs);
}

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

CMorphwizardDoc* CMorphwizardView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMorphwizardDoc)));
	return (CMorphwizardDoc*)m_pDocument;
}
#endif //_DEBUG


const int HISTORY_SIZE=10;
const int PROFILE_SECTION_LENGTH = 10000;
const char PROFILE_PATH[] = "morphwizard.ini";

void CMorphwizardView::LoadHistory()
{
	CString strProfileString;
	CString strKey;
	char strSection[PROFILE_SECTION_LENGTH];
	int iLen = ::GetPrivateProfileSection("History", strSection, PROFILE_SECTION_LENGTH, PROFILE_PATH);
	if (iLen != 0)
	{
		int iSeparatorPos = 0;
		int iPos = 0;
		while(iPos < iLen)
		{
			strProfileString = CString(strSection + iPos);
			iSeparatorPos = strProfileString.Find('=');
			ASSERT(iSeparatorPos != -1);
			strKey = strProfileString.Left(iSeparatorPos);
			m_LastQueries.push_back((const char*)strKey);
			iPos += strProfileString.GetLength() + 1;
		}
	}
}

void CMorphwizardView::SaveHistory()
{
	::WritePrivateProfileSection("History", "", PROFILE_PATH);

	list<string>::iterator pos = m_LastQueries.begin();
	int i = 1;
	while(pos != m_LastQueries.end())
	{
		CString strKey;
		strKey.Format("Query%d", i);
		CString strVal = pos->c_str();
		if (!::WritePrivateProfileString("History", strVal, strKey, PROFILE_PATH))
			break;
		pos++;
		i++;
	}
}

void CMorphwizardView::ChangeHistory(string query)
{
	if (!m_LastQueries.empty())
		if (m_LastQueries.front() == query)
			return;

	m_LastQueries.push_front(query);
	if (m_LastQueries.size() > HISTORY_SIZE)
		m_LastQueries.pop_back();	
	UpdateData(FALSE);
}




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
	CSizeFormView::OnInitialUpdate(); 

	// sizing 
	int GlobalX = ::GetSystemMetrics(SM_CXSCREEN);
	int GlobalY = ::GetSystemMetrics(SM_CYSCREEN);
	const  int DlgWidth = 830;
	const  int DlgHeight = 600;
	GetParent()->SetWindowPos(NULL, (GlobalX-DlgWidth)/2, (GlobalY-DlgHeight)/2-60, DlgWidth, DlgHeight, SWP_SHOWWINDOW|SWP_NOZORDER);
	GetParent()->ShowWindow(SW_MAXIMIZE); // Nick [12/Dec/2003]
	
	m_PredictedList.InsertColumn(PredictGrammemsColumn,"Grammems",LVCFMT_LEFT, 90);
	m_PredictedList.InsertColumn(PredictTypeDictColumn,"Type",LVCFMT_LEFT, 35);
	m_PredictedList.InsertColumn(PredictLemmaColumn,"Source lemma",LVCFMT_LEFT, 90);
	m_PredictedList.InsertColumn(PredictFreqColumnNo,"Freq",LVCFMT_LEFT, 50);
	m_PredictedList.InsertColumn(PredictIndexColumnNo,"Innernumber", LVCFMT_LEFT, 0);
	m_PredictedList.InsertColumn(PredictParadigmColumnNo,"ParadigmNo", LVCFMT_LEFT, 50);
	m_PredictedList.InsertColumn(PredictLemmaPrefixColumnNo,"Prefix", LVCFMT_LEFT, 50);
	m_PredictedList.SetExtendedStyle(LVS_EX_FULLROWSELECT);  

	m_FoundList.InsertColumn(FindParadigmColumnNo,"ParadigmNo",LVCFMT_LEFT, 50);
	m_FoundList.InsertColumn(FindLemmaColumn,"Lemma",LVCFMT_LEFT, 130);
	m_FoundList.InsertColumn(FindGrammemsColumn,"Grammems",LVCFMT_LEFT, 210);
	m_FoundList.InsertColumn(FindTypeGrammemsColumn,"Type",LVCFMT_LEFT, 210);
	m_FoundList.SetExtendedStyle(LVS_EX_FULLROWSELECT);  
	//m_FoundList.AddTip(0, "Tip1");
	//m_FoundList.AddTip(1, "Tip2");

	
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
	else
	{
		//GetDlgItem(IDC_REMOVE)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_ADD)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_EDIT)->SetWindowText("VIEW");
	}

	m_MinimalFrequence = 2;
	m_bOnlyMainPartOfSpeeches = TRUE;


	UpdateData(FALSE);
	
//	CMorphwizardApp* App = (CMorphwizardApp*)AfxGetApp();
//	App->m_PointerCount[this] = 0;
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
	std::string flt_str = (LPCTSTR)tmp;
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
		BYTE pos;
		QWORD grm;
		if	(		!GetWizard()->m_pGramTab->ProcessPOSAndGrammemsIfCan(flt_str.c_str(),&pos,&grm) 
				&&	!GetWizard()->m_pGramTab->ProcessPOSAndGrammemsIfCan(string("* "+flt_str).c_str(),&pos,&grm)
		)
		{
				AfxMessageBox(IDS_BADFSTR,MB_ICONERROR|MB_OK);
		}
		else
		{
			std::string flt_pos;
			if (pos != UnknownPartOfSpeech)
				flt_pos = GetWizard()->m_pGramTab->GetPartOfSpeechStr(pos);
			std::vector<lemma_iterator_t> filter_paradigms;
			for( int i=0; i<found_paradigms.size(); i++ )
			{
				std::string str_pos = GetWizard()->get_pos_string(found_paradigms[i]);

				if (!flt_pos.empty())
					if( flt_pos != str_pos )
						continue;

				if( (GetWizard()->get_all_lemma_grammems(found_paradigms[i]) & grm)  != grm) 
					continue;

				filter_paradigms.push_back(found_paradigms[i]);
			}
			found_paradigms = filter_paradigms;
		}
	};

};

void CMorphwizardView::ShowFoundParadigms() 
{
	m_FoundList.clear();
	
	for( int i=0; i<found_paradigms.size(); i++ )
	{
		int FlexiaModelNo = found_paradigms[i]->second.m_FlexiaModelNo;
		std::string s = Format("%i (%i)", FlexiaModelNo,
			found_paradigms[i]->second.m_AccentModelNo);

		m_FoundList.InsertItem(i, s.c_str() );

		std::string Lemma = GetWizard()->get_lemm_string_with_accents(found_paradigms[i]);
		std::string PrefixSet = GetWizard()->get_prefix_set( found_paradigms[i] );
		if (!PrefixSet.empty())
			Lemma = PrefixSet+'|'+Lemma;
		RmlMakeLower(Lemma, GetWizard()->m_Language);
		m_FoundList.SetItemText(i, FindLemmaColumn, Lemma.c_str());

		s = GetWizard()->get_pos_string(found_paradigms[i]) + " " + GetWizard()->get_grammem_string(found_paradigms[i]);
		m_FoundList.SetItemText(i, FindGrammemsColumn, s.c_str());
			
		s = GetWizard()->get_common_grammems_string(found_paradigms[i]);
		m_FoundList.SetItemText(i, FindTypeGrammemsColumn, s.c_str());

		m_FoundList.m_ToolTips.push_back(GetWizard()->m_FlexiaModels[FlexiaModelNo].m_Comments);

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
		ChangeHistory((const char*)find_what);

		CWizardProgressMeter meter(*GetWizard());

		if(GetCheckedRadioButton(IDC_RFIND_LEM,IDC_WORD_FORM)==IDC_RFIND_LEM)
		{
			std::string s = find_what;
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
			GetWizard()->find_lemm_by_grammem((const char *)find_what, found_paradigms);

		if(GetCheckedRadioButton(IDC_RFIND_LEM,IDC_WORD_FORM)==IDC_WORD_FORM)
		{
			std::string s = find_what;
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
			std::string s = find_what;
			GetWizard()->find_ancodes(s.c_str(), found_paradigms);
		};
		if(GetCheckedRadioButton(IDC_RFIND_LEM,IDC_FIND_BY_USERNAME)==IDC_FIND_BY_USERNAME)
		{
			std::string s = find_what;
			GetWizard()->find_lemm_by_user(s.c_str(), found_paradigms);
		};
		

	// proceed filter string
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


class IsLessForPredict : public binary_function<bool, const size_t&, const size_t&> 
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
					string s1 = W->get_pos_string_and_grammems(S1.m_SourceLemmaAncode);
					string s2 = W->get_pos_string_and_grammems(S2.m_SourceLemmaAncode);
					if ( s1 == s2) return S1.m_Frequence > S2.m_Frequence;
					else
						return s1 < s2;
				};
			case PredictTypeDictColumn: 
				{
					string s1 = W->get_grammem_string(S1.m_SourceCommonAncode);
					string s2 = W->get_grammem_string(S2.m_SourceCommonAncode);
					if ( s1 == s2) return S1.m_Frequence > S2.m_Frequence;
					else
						return s1 < s2;
				};
			case PredictLemmaColumn: 
				{
					string s1 = S1.m_SourceLemma;
					string s2 = S2.m_SourceLemma;
					if ( s1 == s2) return S1.m_Frequence > S2.m_Frequence;
					else
						return s1 < s2;
				};
			case PredictFreqColumnNo: return S1.m_Frequence > S2.m_Frequence;
			case PredictParadigmColumnNo: return S1.m_FlexiaModelNo < S2.m_FlexiaModelNo;
			case PredictLemmaPrefixColumnNo: 
				{
					string s1 = S1.m_PrefixSetStr;
					string s2 = S2.m_PrefixSetStr;
					if ( s1 == s2) return S1.m_Frequence > S2.m_Frequence;
					else
						return s1 < s2;
				};
			default : return _X1 < _X2;
		};
	};

};

void CMorphwizardView::OnPredict() 
{
	try
	{
		UpdateData(TRUE);
		m_FoundList.DeleteAllItems();
		std::vector<lemma_iterator_t> curr_found_paradigms;

		m_PredictedList.clear();

		CString predict_what;
		CString prefix;

		m_PredictWhat.GetWindowText(predict_what);
		std::string Lemma = predict_what;
		int i = predict_what.Find("#");
		
		if (i != -1)
		{
			prefix = predict_what.Mid(0, i);
			predict_what = predict_what.Mid(i+1);
			Lemma = prefix+predict_what;
		};

		GetWizard()->find_lemm(Lemma.c_str(), false, curr_found_paradigms);
		if (!curr_found_paradigms.empty())
			AfxMessageBox ("The dictionary contains already this word!");


		GetWizard()->predict_lemm((const char*)predict_what, m_PredictSuffLength, m_MinimalFrequence, m_bOnlyMainPartOfSpeeches==TRUE);
		if (GetWizard()->m_CurrentPredictedParadigms.empty())
		{
			ErrorMessage("No variants found");
			return;
		};
		vector<size_t> Index;
		for(size_t i=0; i < GetWizard()->m_CurrentPredictedParadigms.size(); i++)
			Index.push_back(i);
		std::sort (Index.begin(), Index.end(), IsLessForPredict(this));
		for(size_t i = 0; i < Index.size(); i++)
		{
			size_t  ind = Index[i];
			const CPredictSuffix& S = *GetWizard()->m_CurrentPredictedParadigms[ind];
			const CFlexiaModel& P = GetWizard()->m_FlexiaModels[S.m_FlexiaModelNo];

			m_PredictedList.InsertItem(i, GetWizard()->get_pos_string_and_grammems(S.m_SourceLemmaAncode).c_str() );
			m_PredictedList.SetItemText(i, PredictTypeDictColumn, GetWizard()->get_grammem_string(S.m_SourceCommonAncode).c_str());
			m_PredictedList.SetItemText(i, PredictLemmaColumn, S.m_SourceLemma.c_str());
			m_PredictedList.SetItemText(i, PredictFreqColumnNo, Format("%i", S.m_Frequence).c_str());
			m_PredictedList.SetItemText(i, PredictIndexColumnNo,  Format("%i", ind).c_str() );
			m_PredictedList.SetItemText(i, PredictParadigmColumnNo,  Format("%i", S.m_FlexiaModelNo).c_str() );
			m_PredictedList.SetItemText(i, PredictLemmaPrefixColumnNo,  S.m_PrefixSetStr.c_str() );
			m_PredictedList.m_ToolTips.push_back(P.m_Comments);
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
			pDocument->m_ParadigmText = GetWizard()->create_slf_from_predicted(atoi(S), common_grammems, 50).c_str();
			pDocument->m_CommonGrammems = common_grammems.c_str();
			int i = predict_what.Find("|");
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
		pDocument->m_ParadigmText = save_slf.c_str();
		pDocument->m_CommonGrammems = save_common_grammems.c_str();
		pDocument->m_Prefixes = save_prefixes.c_str();

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
   	CFileDialog D(FALSE, "txt", "wordlist.txt");
	if (D.DoModal() != IDOK) return;
	FILE * fp = fopen (D.GetPathName(),"wb");

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
   		CFileDialog D(FALSE, "slf", "paradigms.slf");
		if (D.DoModal() != IDOK) return;
		FILE * fp = fopen (D.GetPathName(),"wb");
		if (!fp) 
		{
			AfxMessageBox ("Cannot open file");
			return;
		};
		
		CProgressMeterRML meter;
		meter.SetMaxPos(found_paradigms.size(),100);
		CString info = "Exporting " + D.GetPathName() + "..."; 
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

		if (::MessageBox (0,"Delete all exported paradigms?","MorphWizard",MB_YESNO   ) == IDYES)
		  GetWizard()->delete_checked_lemms();
	}
   catch (...)
   {
	   			AfxMessageBox ("some error has occured");
   };

    
}

//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsImport() 
{
	int line_no = 0;
	CString PathName;

try {
   	CFileDialog D(TRUE, "slf", "paradigms.slf",OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	char file_buffer[10000];
	file_buffer[0] = 0;
	D.m_ofn.lpstrFile = file_buffer;
	D.m_ofn.nMaxFile = 10000;
	if (D.DoModal() != IDOK) return;
	POSITION pos = D.GetStartPosition();
	if (pos == 0) return;
	bool bTestMode = ::MessageBox (0,"Test it or import?","MorphWizard",MB_YESNO   ) == IDYES;
	
	GetWizard()->m_bFullTrace = false;
	PathName = D.GetNextPathName(pos);
	while  (true) // all selected files 
	{

		FILE * fp = fopen (PathName,"r");
		if (!fp) 
		{
			AfxMessageBox ("Cannot open file");
			return;
		};
				
		GetWizard()->log (std::string("import file ") + (const char*)PathName);
				
		CProgressMeterRML meter;
		meter.SetFileMaxPos(fp);
		CString info = (bTestMode?"Test":"Import");
		info+= "ing file " + PathName + "...";
		meter.SetInfo(info);
				
		int ParadigmCount = 0;
		string Errors;
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
						WORD SessionNo = GetWizard()->RegisterSession(P.m_Session);
						GetWizard()->add_lemma(P.m_SlfStr, P.m_TypeGrammemsStr, P.m_PrefixesStr, line_no_err, SessionNo);
					};

					meter.SetFilePos();		  
				}
				catch (CExpc C)
				{
					Errors += Format("%s (%s:%i) \n", C.m_strCause.c_str(), (const char*)PathName, P.m_FirstSlfLineNo+line_no_err);
				}
				catch  (...) 
				{
					Errors += Format("error at %s:%i \n", (const char*)PathName, P.m_FirstSlfLineNo+line_no_err);
				};
					
			}
		}
		fclose (fp);
		if (!Errors.empty())
		{
			try {
				string ErrorFile = MakeFName((const char*)PathName, "err");
				FILE * fp = fopen (ErrorFile.c_str(), "w");
				fprintf (fp, "%s",Errors.c_str());
				fclose(fp);
				ErrorMessage(Format("Errors were written to file %s",  ErrorFile.c_str()));
			} 
			catch (...){
				AfxMessageBox ("Cannot write errors to paradigms.err ");
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
   	CFileDialog D(TRUE, "slf", "paradigms.txt");
	if (D.DoModal() != IDOK) return;
	FILE * fp = fopen (D.GetPathName(),"rb");
	if (!fp) 
	{
		AfxMessageBox ("Cannot open file");
		return;
	};
	char buf[1000];
	std::string strNotFound;
	int ParadigmCount = 0;
	found_paradigms.clear();
	while  (fgets(buf, 1000, fp)) 
	{
		std::string Line = buf;
		Trim(Line);
		if (Line.empty()) continue;
		StringTokenizer tok (Line.c_str(), ";");
		if (!tok())
		{
			std::string mess = std::string("cannot get lemma from ") + buf + std::string("; The format should be <Lemma>;<TypeGrammems>;<MorphPattern>");
			AfxMessageBox (mess.c_str());
			break;
		};
		std::string Lemma = tok.val();
		Trim(Lemma);


		if (!tok())
		{
			std::string mess = std::string("cannot get type grammem ") + buf + std::string("; The format should be <Lemma> <PartofSpeech> <Grammems>");
			AfxMessageBox (mess.c_str());
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
					AfxMessageBox (mess.c_str());
					break;
				};
		};


		if (!tok())
		{
			std::string mess = std::string("cannot get morphological pattern ") + buf + std::string("; The format should be <Lemma>;<TypeGrammems>;<MorphPattern>");
			AfxMessageBox (mess.c_str());
			break;
		};
		std::string FirstCode;
		{
			std::string PosStr = tok.val();
			Trim(PosStr);
			if (!GetWizard()->slf2ancode(PosStr,FirstCode )) 
			{
				std::string mess = std::string("cannot process morph. pattern ") + PosStr;
				AfxMessageBox (mess.c_str());
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
	// TODO: Add your control notification handler code here
    CString FileName = GetWizard()->get_lock_file_name().c_str();
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
	CGriIni cIni("Packing...");;
	cIni.Init(this);
	{
		CWizardProgressMeter meter(*GetWizard());
		GetWizard()->pack();
	}
	AfxMessageBox ("Packing is finished!");
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
extern bool InputBox (const char* Caption, string& OutBuffer);		

//----------------------------------------------------------------------------
void CMorphwizardView::OnToolsSetParaNo()
{
	if (found_paradigms.empty()) 
	{
		ErrorMessage("Cannot set paradigm No to an empty found list");
		return;
	};
	POSITION pos = m_FoundList.GetFirstSelectedItemPosition();		
	WORD ParadigmNo = 0;
	if(pos != 0)
	{
		size_t index = m_FoundList.GetNextSelectedItem(pos);
		assert(index < found_paradigms.size());
		ParadigmNo = found_paradigms[index]->second.m_FlexiaModelNo;
	}
	string s = Format("%i",ParadigmNo);
	if ( !InputBox("Input paradigm No:", s) ) return;
	ParadigmNo = atoi(s.c_str());
	if (ParadigmNo >= GetWizard()->m_FlexiaModels.size())
	{
			ErrorMessage(Format("Paradigm no %i is not found",ParadigmNo));
			return;
	}
	std::string Warn = Format("The program ascribes all found words to paradigm %i . Continue?", ParadigmNo);
	if  (::MessageBox (0, Warn.c_str(),"MorphWizard",MB_OKCANCEL   ) != IDOK )
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

/*
void CMorphwizardView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CSizeFormView::OnActivate(nState, pWndOther, bMinimized);
	SetInputLanguage(GetWizard()->m_Language);
}
*/

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
	if( !m_pFreqDict ) 
		m_pFreqDict = new FreqDict;

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
	CSizeFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}


//////////////////////////////////////////////////////////////////////////////
// FreqDict class implementation
//////////////////////////////////////////////////////////////////////////////
bool FreqDict::Load( const CString&  fileName, int minFreq )
{
	if( fileName==m_fileName && minFreq==m_minFreq ) 
		return false;

	try
	{
		CStdioFile file;
		if( file.Open(fileName,CFile::modeRead) ) 
		{
			CString line;
			file.ReadString(line);
			file.ReadString(line);
			file.ReadString(line);
			while( file.ReadString(line) ) 
			{
				FreqWord word;
				int i = line.Find(" ");
				if( i<0 ) continue;
				word.freq = atoi((line.Left(i)).Trim());
				static_cast<std::string&>(word) = (LPCSTR)line.Mid(i+1);
				Trim(word);
				if( !!word ) 
					m_words.push_back(word);
			}
		}

		if( !m_words.empty() ) 
			return true;
	}
	catch( ... ) 
	{
	}
	m_fileName.Empty();
	m_words.clear();
	return false;
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
	string Caption = Format("Input comments to paradigm No:%i", ModelNo);
	InputBox(Caption.c_str(), GetWizard()->m_FlexiaModels[ModelNo].m_Comments);
}
