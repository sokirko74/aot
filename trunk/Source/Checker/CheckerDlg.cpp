// CheckerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Checker.h"
#include "CheckerDlg.h"
#include "ProceedDlg.h"
#include "../MorphCorpusCreator/MorphCorpusCreator.h"

#include "GriThrPB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HISTORY_SIZE				5
#define PROFILE_SECTION_LENGTH		10000
#define PROFILE_PATH				"rml_checker.ini"


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP


}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckerDlg dialog

CCheckerDlg::CCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckerDlg)
	m_from = 1;
	m_numb = 99999;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bLoadCOM = true;
	LoadHistory();
}
CCheckerDlg::~CCheckerDlg() 
{
	SaveHistory();
}



CString CCheckerDlg::GetCurrFileName() const 
{
	CString FileName;
	m_comboHistory.GetWindowText(FileName);
	return FileName;
}


void CCheckerDlg::LoadHistory()
{
	CString strProfileString;
	CString strKey;
	TCHAR strSection[PROFILE_SECTION_LENGTH];
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
			m_LastQueries.push_back(strKey);
			iPos += strProfileString.GetLength() + 1;
		}
	}
}

void CCheckerDlg::SaveHistory()
{
	CString strVal;
	::WritePrivateProfileSection("History", "", PROFILE_PATH);
	list<CString>::iterator pos = m_LastQueries.begin();
	CString strKey;
	int i = 1;
	while(pos != m_LastQueries.end())
	{
		strKey.Format("Query%d", i);
		strVal = *pos;
		if (!::WritePrivateProfileString("History", strVal, strKey, PROFILE_PATH))
			break;
		pos++;
		i++;
        if (i > HISTORY_SIZE) break;
	}
}

void CCheckerDlg::ChangeHistory(const CString& query)
{
	m_LastQueries.push_front(query);
	if (m_LastQueries.size() > HISTORY_SIZE)
		m_LastQueries.pop_back();	
	UpdateData(FALSE);
}


void CCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckerDlg)
	DDX_Text(pDX, IDC_FROM, m_from);
	DDX_Text(pDX, IDC_NUMB, m_numb);
	DDX_Control(pDX, IDC_COMBO1, m_comboHistory);
	//}}AFX_DATA_MAP
	//  init history

	m_comboHistory.ResetContent();
	list<CString>::iterator pos = m_LastQueries.begin();
	while (pos != m_LastQueries.end())
	{
		m_comboHistory.AddString(*pos);
		pos++;
	}
	
    if (!m_LastQueries.empty())
    {
        m_comboHistory.SetCurSel(0);
        m_comboHistory.SelectString(-1, *m_LastQueries.begin());
    }


	
}

BEGIN_MESSAGE_MAP(CCheckerDlg, CDialog)
	//{{AFX_MSG_MAP(CCheckerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_B1, OnOpenFile)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_DELETE_BASE, OnBnClickedDeleteBase)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckerDlg message handlers

BOOL CCheckerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
// TODO: Add extra initialization here
	return TRUE;
}

void CCheckerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCheckerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CCheckerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////

void CCheckerDlg::OnOpenFile() 
{
	CString  FileName = GetCurrFileName();
	CFileDialog cDlg(TRUE,NULL,FileName,
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", AfxGetMainWnd());
	cDlg.m_ofn.lpstrTitle = " Input File:";

	if( cDlg.DoModal() != IDOK )
		return;

	ChangeHistory(cDlg.GetPathName());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BOOL CCheckerDlg::InitCom()
{
	try 
	{
// инициализируем библиотеку COM
	CoInitialize(NULL);
	if (!m_bLoadCOM) return TRUE;
// создаем семантический анализ (ISemStructurePtr)
	piSeman.CreateInstance( __uuidof(SEMANLib::SemStructure));
	clock_t     start = clock();

// инициализация досемантических словарей
	piSeman->InitPresemanDicts();
// инициализация семантических словарей
	piSeman->InitSemanDicts();
	piSeman->InitializeIndices();
    piSeman->ShouldBuildTclGraph = FALSE;
	clock_t finish = clock();
	double timespan = finish - start; 
	m_GerSyntaxHolder.LoadSyntax(morphGerman);
	m_RusSyntaxHolder.LoadSyntax(morphRussian);
	} catch(...)
	{
		AfxMessageBox("SEMAN initialization failed!");
		return FALSE;
	}

	m_Language = morphRussian;
	

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CCheckerDlg::ExitCom()
{
	piSeman = NULL;
	m_GerSyntaxHolder.DeleteProcessors();
	m_RusSyntaxHolder.DeleteProcessors();
	CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCheckerDlg::LoadData()
{
	FILE *fi;
	if( (fi=fopen(GetCurrFileName(),"rt"))==NULL )
	{
		CString s;
		AfxFormatString1(s,IDS_OPNERR,GetCurrFileName());
		AfxMessageBox(s);
		return FALSE;
	}


	char tmp[8196];
	
	while( fgets(tmp,8196,fi)!=NULL )
	{
		string s = tmp;
		Trim(s);
		if (s.empty())
		{
			//AfxMessageBox(Format("Cannot process line No %i in %s (it is empty!)", m_CheckExamples.size() + 1, m_file).c_str());
			//return FALSE;

			// skipping  empty lines 
			continue;
		};
		for (size_t i=0; i <s.length(); i++)
			if (s.substr(i,2) == "\\n")
				s.replace(i,2,"\r\n");

		string Comments;

		{
			int k = s.find("//");
			if (k != string::npos)
			{
				Comments = s.substr(k+2);
				s.erase(k);
			};
		};
		CSourceExample E;
		E.m_Text = s;
		E.m_Comments = Comments;
		m_CheckExamples.push_back(E);

		
	}

	fclose(fi);
	m_CheckerType = TranslatorCheck;
	m_Language = morphRussian;
	fi = fopen (MakeFName((const char*)GetCurrFileName(), "opt").c_str(), "r");
	m_CommandLine = "";
	m_ExternalOutFile = "";
	m_ExternalCatalog = "";
	m_bResultsAreSmall = false;
	if (fi)
	{
		char s[1024];
		while (fgets(s,1024,fi))
		{
			char* q = strtok(s," \t\n\r");
			if (q)
			{
				if (!stricmp ("SyntaxCheck", q))
				{
					m_CheckerType = SyntaxCheck;
				}
				if (!stricmp ("PostMorphCheck", q))
				{
					m_CheckerType = PostMorphCheck;
				}
				if (!stricmp ("RusSynthesis", q))
				{
					m_CheckerType = RusSynthesisCheck;
				}
				if (!stricmp ("SentenceBreaker", q))
				{
					m_CheckerType = SentenceBreakerCheck;
				}
				if (!stricmp ("Prediction", q))
				{
					m_CheckerType = PredictionCheck;
				}
				if (!stricmp ("MorphCorpusCheck", q))
				{
					m_CheckerType = MorphCorpusCheck;
				}
				if (!stricmp ("ExternalExeFileCheck", q))
				{
					m_CheckerType = ExternalExeFileCheck;
				}
				if (!stricmp ("ResultsAreSmall", q))
				{
					m_bResultsAreSmall = true;
				}
				

				MorphLanguageEnum langua;
				if (GetLanguageByString(q, langua))
					m_Language = langua;

				if (!stricmp ("ExternalCommandLine", q))
				{
					q = strtok(NULL,"\r\n");
					m_CommandLine = q;
				}

				if (!stricmp ("ExternalOutFile", q))
				{
					q = strtok(NULL,"\r\n");
					m_ExternalOutFile = q;
				}

				if (!stricmp ("ExternalCatalog", q))
				{
					q = strtok(NULL,"\r\n");
					m_ExternalCatalog = q;
				}
				
			};
			
		};

		fclose(fi);
	};

	if (m_CheckerType == ExternalExeFileCheck)
	{
		if (m_CommandLine.empty()) 
		{
			ErrorMessage ("No definition of ExternalCommandLine in options file");
			return FALSE;
		};
		if (m_ExternalOutFile.empty()) 
		{
			ErrorMessage ("No definition of ExternalOutFile in options file");
			return FALSE;
		};
		
	}
	return TRUE;
}

string CCheckerDlg::GetBaseFileName() const
{
	return MakeFName((const char*)GetCurrFileName(), "base");
};
/////////////////////////////////////////////////////////////////////////////

BOOL CCheckerDlg::LoadBase()
{
	m_changed = FALSE;

	
	
	FILE *fb;
	if( (fb=fopen(GetBaseFileName().c_str(),"rt"))==NULL )
	{
		/*if( AfxMessageBox(IDS_NOBASE,MB_ICONQUESTION|MB_YESNO)!=IDYES )
			return FALSE;
		else*/
		return TRUE;
	}

	char tmp[40000];
	int LineNo = 0;
	while( fgets(tmp,40000,fb)!=NULL )
	{
		LineNo++;
		string line  = tmp;
		if (line.empty()) continue;
		if( line[0] != '[')
		{
			ErrorMessage (Format("Cannot read base at line %i (no open bracket)",LineNo));
			fclose(fb);
			return FALSE;
		}
		int index = line.find("] ");
		if( index == string::npos)
		{
			ErrorMessage (Format("Cannot read base at line %i (no close bracket)",LineNo));
			fclose(fb);
			return FALSE;
		}
		CBase base;
		base.num = atoi(line.c_str() + 1);
		if (base.num == 0)
		{
			ErrorMessage (Format("Cannot read base at line %i (base num is zero)",LineNo));
			fclose(fb);
			return FALSE;
		}
		base.txt = line.substr(index + 2);
		TrimRight(base.txt);
		m_base.push_back(base);
	}
	fclose(fb);

	sort(m_base.begin(),m_base.end());
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCheckerDlg::SaveBase()
{
	if( !m_changed )
		return TRUE;

	
	FILE *fb;
	if( (fb=fopen(GetBaseFileName().c_str(),"w+t"))==NULL )
	{
		CString s;
		AfxFormatString1(s,IDS_OPNERR,GetBaseFileName().c_str());
		AfxMessageBox(s);
		return FALSE;
	}

	for( int i=0; i<m_base.size(); i++ )
		fprintf(fb,"[%03i] %s\n",m_base[i].num,m_base[i].txt.c_str());
	fclose(fb);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CCheckerDlg::OnOK() 
{
	GetDlgItemText(IDC_EDIT1,GetCurrFileName());
	m_from = GetDlgItemInt(IDC_FROM);
	if (m_from < 1)  m_from = 1;
	m_numb = GetDlgItemInt(IDC_NUMB);
//
	m_CheckExamples.clear();
	m_base.clear();
	m_tran.clear();
	m_nbad.clear();
//
	if( !LoadData() )
		return;
	if( !LoadBase() )
		return;
//
	if( !Operate() )
		return;
//
	Proceed();
	SaveBase();
}

/////////////////////////////////////////////////////////////////////////////


string GetGramInfo (IGramTabPtr  A, IParadigmPtr& F)
{
	string tab_str = (const char*)F->SrcAncode;
	short   PartOfSpeech = A->GetPartOfSpeech (tab_str.c_str());
	string  Result	= (const char*)A->GetPartOfSpeechStr(PartOfSpeech) + string(" ") ;	
	string  CommonAncode = (const char*)F->TypeAncode;
	if (!CommonAncode.empty() && CommonAncode[0] != '?')
	{
		Result +=  (const char*)A->GrammemsToStr (A->GetGrammems(_bstr_t(CommonAncode.c_str())));
		Result +=  "| ";
	};

	for (long i=0; i < tab_str.length(); i+=2)
	{
		
		QWORD G = A->GetGrammems(_bstr_t(tab_str.c_str()+i));
		Result += (const char*)A->GrammemsToStr (G) + string(";");
	};
	return Result;
}

string GetStringByMorph(MorphLanguageEnum	Language, IParadigmCollectionPtr P, IGramTabPtr GramTab)
{
	string Result;
	for (size_t i=0; i< P->Count; i++)
	{
		IParadigmPtr F =  P->Item[i];
		Result +=  F->Founded ? "+" : "-";
		Result +=  " ";
		Result +=  (const char*)F->Form[0];
		Result +=  " ";
		Result +=  GetGramInfo(GramTab,F);
		Result +=  "\x1";
	};
	return Result;
};


string GetStringBySyntax(MorphLanguageEnum	Language, IUnknown* P, IGramTabPtr GramTab)
{
	ISentencesCollectionPtr Synan = P;
	
	int nSent = Synan->SentencesCount;
	if (nSent == 0)
	{
		return " No sentence";
	};
	StringSet Groups;

	for (int SentenceNo = 0; SentenceNo < nSent; SentenceNo++)
	{
		ISentencePtr piSentence = Synan->Sentence[SentenceNo];
		int iWord=0,iClau=0,iCvar=0;
		

		try {
		
			int nClausesCount = piSentence->PrimitiveClausesCount;
			for( int WordNo = 0; WordNo<piSentence->WordsNum; WordNo++ )
			{
				IWordPtr W = piSentence->Word[WordNo];

				if (W->MainVerbsCount > 0)
				{
					string S = string("AnalytForm\t=\t")+  (const char*)W->GetWordStr();
					for (size_t i=0; i< W->MainVerbsCount; i++)
					{
						S += string(" ") + (const char*)piSentence->Word[W->MainVerbs[i]]->GetWordStr();

						IWordPtr W1 = piSentence->Word[W->MainVerbs[i]];
						for (size_t j=0; j< W1->MainVerbsCount; j++)
							S += string(" ") + (const char*)piSentence->Word[W1->MainVerbs[j]]->GetWordStr();

					};
					Groups.insert(S);
				}

				if (W->HomonymsCount > 1)
				{
					
					vector<string>	 Homonyms;
					for (size_t i=0; i< W->HomonymsCount; i++)
					{
						IHomonymPtr H = W->Homonym[i];
						Homonyms.push_back(string((const char*)H->Lemma) + string("[")+(const char*)H->POSStr+string("]"));
					};
					sort (Homonyms.begin(), Homonyms.end());
					string S = string("Homonyms\t=\t")+  (const char*)W->GetWordStr();
					for (size_t i=0; i< Homonyms.size(); i++)
						S += string(" ") + Homonyms[i];
					Groups.insert(S);
				};
				
			};

			for( int ClauseNo = 0; ClauseNo<nClausesCount; ClauseNo++ )
			{
				IClausePtr piClause = piSentence->GetClause(ClauseNo);
				int nCvar = piClause->GetVariantsCount();
				if (nCvar == 0)
				{
					ErrorMessage (" a clause with no variants is found");
					continue;
				};

				int nVmax = piClause->ClauseVariant[0]->VariantWeight;
				if (piClause->RelativeWord != -1)
				{
						string S = string("RelativeWord\t=\t")+  (const char*)piSentence->Word[piClause->RelativeWord]->GetWordStr();
						Groups.insert(S);
				};
				if (piClause->AntecedentWord != -1)
				{
						string S = string("AntecedentWord\t=\t")+  (const char*)piSentence->Word[piClause->AntecedentWord]->GetWordStr();
						Groups.insert(S);
				};
				
				for( int iCvar=0; iCvar<nCvar; iCvar++ )
				{
					IClauseVariantPtr V = piClause->ClauseVariant[iCvar];
					if( V->VariantWeight < nVmax ) break;
					
					for (size_t i =0; i < V->SubjectsCount; i++)
					{
						int WordNo  = V->Unit[V->Subjects[i] ]->WordNum;
						IWordPtr piSubjWord = piSentence->Word[WordNo];
						string S = string("SUBJ\t=\t")+  (const char*)piSubjWord->GetWordStr();
						Groups.insert(S);
					};

					if (V->Predicate != -1)
					{
						int  WordNo  = V->Unit[V->Predicate]->WordNum;
						IWordPtr piPredWord = piSentence->Word[WordNo];
						string S = string("PREDICATE\t=\t")+  (const char*)piPredWord->GetWordStr();
						for (size_t i=0; i< piPredWord->MainVerbsCount; i++)
						{
							S+= string("-") + (const char*)piSentence->Word[piPredWord->MainVerbs[i]]->GetWordStr();
						};

						Groups.insert(S);
					};
					// распечатываем клаузу

					string S = GetClauseTypeDescr(Language, piClause, V->ClauseRootNo);
					S += "\t=\t";
					for (int WordNo = piClause->FirstWord; WordNo <= piClause->LastWord; WordNo++)
					{
						IWordPtr piWord = piSentence->Word[WordNo];
						if (piWord->bDeleted == FALSE) 
						{
							S += (const char*)piWord->GetWordStr();
							S += " ";
						};
					};
					Groups.insert(S);

					// распечатываем все группы 
					

					for (int GroupNo = 0; GroupNo < V->GroupsCount; GroupNo++)
					{
						SYNANLib::IGroupPtr G  = V->Group[GroupNo];
						
						S = (const char*)G->TypeStr;
						S += "\t=\t";
						for (int WordNo = G->FirstWord; WordNo <= G->LastWord; WordNo++)
						{
							IWordPtr piWord = piSentence->Word[WordNo];
							if (piWord->bDeleted == FALSE) 
							{
								S += (const char*)piWord->GetWordStr();
								S += " ";
							}
						};
						Groups.insert(S);
					};
					
				};

			};
		} 
		catch(...)
		{
			return "an exception occured";
		}
	}; //  cycle for sentences
	string Result;
	for (StringSet::const_iterator it = Groups.begin(); !(it == Groups.end()); it++)
	{
		Result += *it;
		Result += "\x1";
	};

	return Result;

};


string GetStringBySentenceBreaker (GRAPHANLib::IGraphmatFilePtr piGraphan)
{
	int CountOfSentences = 1;
	string Result, Line;

	for (int i=1; i<piGraphan->GetLineCount(); i++)
	{
		if (			!piGraphan->HasDescr(i, GRAPHANLib::OEOLN)
				&&		!piGraphan->HasDescr(i, GRAPHANLib::OSpc)
			)
		{
			Line += (const char*)piGraphan->GetWord(i);
			Line += " ";
		};
		if (piGraphan->HasDescr(i, GRAPHANLib::OSentEnd))
		{
			Result += Format("Sent%i\t=\t",CountOfSentences++);
			Result += Line;
			Result += "\x1";
			Line = "";
			
			
		};
	};
	return Result;
};

std::string  MakeSyntaxCheck (CCOMSyntaxHolder& SyntaxHolder, std::string Input)
{
	SyntaxHolder.GetSentencesFromSynAn(Input.c_str(), FALSE, FALSE, FALSE);
	return GetStringBySyntax(SyntaxHolder.m_CurrentLanguage, SyntaxHolder.m_piSentCollection, SyntaxHolder.m_pGramTab);
}

std::string  MakePostMorphCheck (CCOMSyntaxHolder& SyntaxHolder, std::string Input)
{
    SyntaxHolder.m_piMAPost->bHumanFriendlyOutput = TRUE; 
    SyntaxHolder.BuildBeforeSyntax(Input.c_str(), FALSE, FALSE, FALSE);
    SyntaxHolder.m_piMAPost->bHumanFriendlyOutput = FALSE;
    string Result;
    for (size_t i=0; i < SyntaxHolder.m_piBeforeSyntaxPlmLines->Count; i++)
    {
        Result += (const char*)SyntaxHolder.m_piBeforeSyntaxPlmLines->Item[i];
        Result += "\x1";
    }
	return Result;
}

string  MakeMorphCorpusCheck (CCOMSyntaxHolder& SyntaxHolder, std::string Input)
{
	CMorphCorpusCreator C;
	SyntaxHolder.GetSentencesFromSynAn(Input.c_str(), FALSE, FALSE, FALSE);
	return  C.get_xml_result(SyntaxHolder);
}


static UINT MakeProc(LPVOID pArg)
{
	CCheckerDlg *pDlg = (CCheckerDlg *)pArg;

	CoInitialize(NULL);

	

	vector<string> SynCollocCheckResults;
	if ( ExternalExeFileCheck == pDlg->m_CheckerType)
	{
		if (!pDlg->PerformExternalExeFileCheck(SynCollocCheckResults))
		{
			CoUninitialize();
			theTpb->EndDone();
			return 1;
		};
		// ignore  user choice if ExternalExeFileCheck
		pDlg->m_from  = 1;
		pDlg->m_numb  = 99999;
	};


	for( int i=0; i<pDlg->m_from-1; i++ )
		pDlg->m_tran.push_back("");

	int size = pDlg->GetNumberOfExamplesToCheck();
	
	for( int i=0; i<size; i++ )
	{
		string Result;
		int SentenceNo = i+pDlg->m_from-1;
        if (SentenceNo >= pDlg->m_CheckExamples.size())
        {
            AfxMessageBox("Sentence ID is out of bounds!");
            continue;
        }
		try 
		{
			if( theTpb->IsBreak() )
			{
				
				CoUninitialize();
				theTpb->EndExit();
				return( 0 );
			}
			
			char hdr[128];
			sprintf(hdr," Processing sentence %i of %i",i+1,size);
			theTpb->SetHead(hdr);
			theTpb->Percent(i*100/size);
	//
			;
			switch (pDlg->m_CheckerType) {
                
			case  PostMorphCheck:
						{
							Result  = MakePostMorphCheck
								(
									(pDlg->m_Language == morphGerman)?pDlg->m_GerSyntaxHolder:pDlg->m_RusSyntaxHolder,
									pDlg->m_CheckExamples[SentenceNo].m_Text
								);
							break;
						}
			case  SyntaxCheck:
						{
							Result  = MakeSyntaxCheck
								(
									(pDlg->m_Language == morphGerman)?pDlg->m_GerSyntaxHolder:pDlg->m_RusSyntaxHolder,
									pDlg->m_CheckExamples[SentenceNo].m_Text
								);
							break;
						}
			case  ExternalExeFileCheck:
						{
							Result = SynCollocCheckResults[i];
							break;
						}
			case  MorphCorpusCheck:
						{
							Result = MakeMorphCorpusCheck
								(
									(pDlg->m_Language == morphGerman)?pDlg->m_GerSyntaxHolder:pDlg->m_RusSyntaxHolder,
									pDlg->m_CheckExamples[SentenceNo].m_Text
								);

							for (size_t i=0; i  <  Result.length(); i++)
								if (Result[i] == '\n')
									Result[i] = '\x1';

							break;
						}
			case  PredictionCheck:
						{
							IParadigmCollectionPtr P;
							string WordForm  = pDlg->m_CheckExamples[SentenceNo].m_Text;
							Trim(WordForm);
							if (pDlg->m_Language == morphGerman)
							{
								P = pDlg->m_GerSyntaxHolder.m_piLemmatizer->CreateParadigmCollectionFromForm(WordForm.c_str(), TRUE, TRUE);
								Result  = GetStringByMorph(morphGerman, P, pDlg->m_GerSyntaxHolder.m_pGramTab);
							}
							else
							{
								P = pDlg->m_RusSyntaxHolder.m_piLemmatizer->CreateParadigmCollectionFromForm(WordForm.c_str(), TRUE, TRUE);
								Result  = GetStringByMorph(morphRussian, P, pDlg->m_RusSyntaxHolder.m_pGramTab);
							};
							break;
						}
			case  SentenceBreakerCheck:
						{
							GRAPHANLib::IGraphmatFilePtr piGraphan;
							if (pDlg->m_Language != morphGerman)
								piGraphan = pDlg->m_RusSyntaxHolder.m_piGraphan;
							else
								piGraphan = pDlg->m_GerSyntaxHolder.m_piGraphan;
							piGraphan->LoadStringToGraphan(pDlg->m_CheckExamples[SentenceNo].m_Text.c_str());
							Result  = GetStringBySentenceBreaker(piGraphan);
							break;
						}
			case TranslatorCheck:
						{
							string SemGraph = (const char*)pDlg->piSeman->FindSituations(pDlg->m_CheckExamples[SentenceNo].m_Text.c_str(),0,"общ",20000,-1,"");
							string SynGraph = (const char*)pDlg->piSeman->TranslateToEnglish();
							Result  = (const char*)pDlg->piSeman->BuildSentence();
							break;
						};
			case RusSynthesisCheck:
						{
							string SourceText = pDlg->m_CheckExamples[SentenceNo].m_Text;
							string LemmasToReplace;
							{
								int index = SourceText.find("##");
								if (index != string::npos)
								{
									LemmasToReplace = SourceText.substr(index+2);
									SourceText.erase(index);
								}
							}
							string Question;
							{
								int index = SourceText.find("#mem");
								if (index != string::npos)
								{
									Question = SourceText.substr(index+4);
									SourceText.erase(index);
								};

							}

							
							Result = (const char*)pDlg->piSeman->FindSituations(SourceText.c_str(),0,"общ",20000,-1,"");
							pDlg->piSeman->SetLemmasToReplace(LemmasToReplace.c_str());
							if (!Question.empty())
							{
								pDlg->piSeman->SaveThisSentence();
								pDlg->piSeman->FindSituations(Question.c_str(),0,"общ",20000,-1,"");
								Result  = pDlg->piSeman->Answer();
							}
							else
								Result  = (const char*)pDlg->piSeman->SyntRusSentence();
							break;
						};
			};
		}
		catch (...)
		{
			CString S;
			S.Format("Cannot process the sentence N %i", SentenceNo+1);
			AfxMessageBox (S);
			theTpb->EndExit();
			return 0;
		};



		pDlg->m_tran.push_back(Result);
	
		CBase b;
		b.num = SentenceNo+1;
		b.txt = Result;
		if( !binary_search(pDlg->m_base.begin(),pDlg->m_base.end(),b) )
		{
			bool bDebug = binary_search(pDlg->m_base.begin(),pDlg->m_base.end(),b);
			pDlg->m_nbad.push_back(i+pDlg->m_from-1);
		}
		

	}


	CoUninitialize();
	theTpb->EndDone();
	return( 1 );
}

int CCheckerDlg::GetNumberOfExamplesToCheck() const 
{
	return  min(m_CheckExamples.size()-m_from+1,m_numb);
};

string change_x1_to_cr(string s)
{
	string b;
	for (size_t i=0; i < s.length(); i++)
		if (s[i] == 1)
			b += "\r\n";
		else
			b += s[i];
	return b;
}

bool CCheckerDlg::PerformExternalExeFileCheck(vector<string>& Result)
{
	vector<string> InputFiles;
	InputFiles.push_back( (const char*)GetCurrFileName() );
    
	m_CheckExamples.clear();
	Result.clear();

	if (!m_ExternalCatalog.empty())
	{
		struct _finddata_t c_file;
		long hFile;
		if( (hFile = _findfirst( m_ExternalCatalog.c_str(), &c_file )) == -1L )
		{
			ErrorMessage(Format("No input file  in %s", m_ExternalCatalog.c_str()) );
			return false;
		};
		InputFiles.clear();
		do 
		{
			InputFiles.push_back( GetPathByFile(m_ExternalCatalog) + c_file.name );
		}
		while (!m_ExternalCatalog.empty() && _findnext( hFile, &c_file ) == 0);
		sort (InputFiles.begin(), InputFiles.end());
	};


	for(int FileNo=0; FileNo < InputFiles.size();FileNo++)
	{
		string InputFile = InputFiles[FileNo];

		string CommandLine = Format(m_CommandLine.c_str(), InputFile.c_str());
		system (CommandLine.c_str());
		FILE * fp = fopen(m_ExternalOutFile.c_str(), "r");
		if (!fp)
		{
			ErrorMessage(Format("Cannot open output file %s", m_ExternalOutFile.c_str()) );
			return false;
		};
		char buffer[10000];
		bool bFirstResultIsFound = false;
		bool bFirstLineIsFound = false;

		while (fgets(buffer,10000, fp))
		{
			rtrim(buffer);
			if (buffer[0] != '\t')
			{
				CSourceExample E;
				E.m_Text = change_x1_to_cr(buffer);
				if (m_ExternalCatalog.empty() || m_CheckExamples.empty() || !bFirstLineIsFound) 
				{
					m_CheckExamples.push_back(E);
					Result.push_back("");
					bFirstLineIsFound = true;
				}
				else
				if (!bFirstResultIsFound)
				{

					m_CheckExamples.back().m_Text += string(" ") + change_x1_to_cr(buffer);
				}
			}
			else
			{
				Result.back() += buffer + string("\x1");
				bFirstResultIsFound = true;
			}
		};
		fclose (fp);
	}
	

		
	
	return true;
};



/////////////////////////////////////////////////////////////////////////////

BOOL CCheckerDlg::Operate() 
{
	CTime C_begin = CTime::GetCurrentTime();

	theTpb = new CThrPB(this,MakeProc,this);
	BOOL bRet = theTpb->DoModal();

	if (bRet)
		if (m_CheckerType == TranslatorCheck)
		{
			CString S;
			CTime C_end = CTime::GetCurrentTime();
			CTimeSpan t = C_end - C_begin;
			S.Format ("Elapsed time:\n %s\n Total time = %s", 
				(const char*)piSeman->ModuleTimeStatistics,
				t.Format ("%M minutes %S seconds"));
			
			AfxMessageBox (S);
		};

	delete theTpb;
	theTpb = NULL;

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////

void CCheckerDlg::Proceed()
{
	if( m_nbad.size()==0 )
	{
		AfxMessageBox("No differences!",MB_ICONINFORMATION|MB_OK);
		return;
	}

	CProceedDlg dlg;
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////


void CCheckerDlg::OnBnClickedDeleteBase()
{
	if( AfxMessageBox(IDS_PROMPT_DELETE_BASE,MB_ICONQUESTION|MB_YESNO)!=IDYES )
		return;
	remove(GetBaseFileName().c_str());
	
}
