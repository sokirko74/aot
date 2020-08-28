// demoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "demo.h"
#include "vector"
#include "assert.h"
#include "demoDlg.h"


#include "Splash.h"


#define INLANG_ALL		0
#define INLANG_RUSSIAN	1
#define INLANG_ENGLISH	2
#define INLANG_GERMAN	3


#define COLUMN_LANGUAGE	0
#define COLUMN_STATUS	1
#define COLUMN_TYPE		2
#define COLUMN_NORM		3
#define COLUMN_FORM		4
#define COLUMN_ID		5
#define COLUMN_HOMO		6
#define COLUMN_WORD		7
#define COLUMN_ANCODE	8
#define COLUMN_ACCENTS	9




/////////////////////////////////////////////////////////////////////////////
// CDemoDlg dialog

CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDemoDlg::IDD, pParent)
{
	m_pToolTip = NULL;
	//{{AFX_DATA_INIT(CDemoDlg)
	m_strSrc = _T("");
	m_iLang = 0;
	m_iSubject = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDemoDlg::~CDemoDlg()
{
	if (m_pToolTip)
		delete m_pToolTip;
}


void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemoDlg)
	DDX_Control(pDX, IDOK, m_buttonExit);
	DDX_Control(pDX, IDC_BUTTON_FIND, m_buttonFind);
	DDX_Control(pDX, IDC_LIST_RESULTS, m_ctrlResults);
	DDX_Text(pDX, IDC_EDIT_SRC, m_strSrc);
	DDX_Radio(pDX, IDC_RADIO_ALL, m_iLang);
	DDX_Radio(pDX, IDC_RADIO_FINANCE, m_iSubject);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CDemoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnButtonFind)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_RADIO_LITERATURE, OnRadioLiterature)
	ON_BN_CLICKED(IDC_RADIO_FINANCE, OnRadioFinance)
	ON_BN_CLICKED(IDC_RADIO_COMPUTER, OnRadioComputer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemoDlg message handlers

BOOL CDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	std::string strCause;
	if (!IsRmlRegistered(strCause))
	{
		ErrorMessage(strCause);
		return false;
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

//	ToolTip init
	m_pToolTip = new CToolTipCtrl;
	m_pToolTip->Create(this);
	m_pToolTip->AddTool(&m_buttonExit, _T("Exit"));
	m_pToolTip->AddTool(&m_buttonFind, _T("Find"));
//	Buttons init
	HICON	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICONEXIT), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	m_buttonExit.SetIcon(hIcon);
	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICONFIND), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	m_buttonFind.SetIcon(hIcon);
//	Column init
	CRect rect;
	m_ctrlResults.GetWindowRect(&rect);
	m_ctrlResults.InsertColumn(COLUMN_LANGUAGE, _T("Lang"), LVCFMT_LEFT, rect.Width() / 14, COLUMN_LANGUAGE); 
	m_ctrlResults.InsertColumn(COLUMN_STATUS, _T("Status"), LVCFMT_LEFT, rect.Width() / 10, COLUMN_STATUS); 
	m_ctrlResults.InsertColumn(COLUMN_TYPE, _T("Type"), LVCFMT_LEFT, rect.Width() / 12, COLUMN_TYPE); 
	m_ctrlResults.InsertColumn(COLUMN_NORM, _T("Normal form"), LVCFMT_LEFT, rect.Width() / 4, COLUMN_NORM); 
	m_ctrlResults.InsertColumn(COLUMN_FORM, _T("Information"), LVCFMT_LEFT, rect.Width() / 4, COLUMN_FORM); 
	m_ctrlResults.InsertColumn(COLUMN_ID, _T("ID / Source"), LVCFMT_LEFT, rect.Width() / 4, COLUMN_ID);
	m_ctrlResults.InsertColumn(COLUMN_HOMO, _T("HomoWeight"), LVCFMT_LEFT, rect.Width() / 4, COLUMN_HOMO);
	m_ctrlResults.InsertColumn(COLUMN_WORD, _T("WordWeight"), LVCFMT_LEFT, rect.Width() / 4, COLUMN_WORD);
	m_ctrlResults.InsertColumn(COLUMN_ANCODE, _T("Ancode"), LVCFMT_LEFT, rect.Width() / 4, COLUMN_ANCODE);
	m_ctrlResults.InsertColumn(COLUMN_ACCENTS, _T("Accents"), LVCFMT_LEFT, rect.Width() / 4, COLUMN_ACCENTS);
	

//	Lemmatizer init
	MorphLanguageEnum Language = ((CDemoApp*)AfxGetApp())->m_Language;
	if ( (Language == morphUnknown) || (Language == morphEnglish))
	{
		HRESULT hr = m_piEnglish.CreateInstance(__uuidof(LEMMATIZERLib::LemmatizerEnglish));
		if (FAILED(hr))
		{
			AfxMessageBox(_T("Fatal Error: Lemmatizer creation failed"));
			return FALSE;
		}
		hr = m_piEnglishAgram.CreateInstance(__uuidof(AGRAMTABLib::EngGramTab));
		if (FAILED(hr))
		{
			AfxMessageBox(_T("Fatal Error: English agramtab creation failed"));
			return FALSE;
		}
		m_bEngAgramLoaded = false;	
		m_bEnglishLoaded = false;
		hr = m_piEnglish->raw_LoadDictionariesRegistry();
		if (!FAILED(hr))
		{
			m_bEnglishLoaded=true;
			hr = m_piEnglishAgram->raw_Load();
			if (!FAILED(hr))
				m_bEngAgramLoaded=true;
		}
		m_piEnglish->raw_LoadStatisticRegistry(LEMMATIZERLib::idlFinance);
		m_piEnglish->UseStatistic = TRUE;
	};


	if ( (Language == morphUnknown) || (Language == morphGerman))
	{
		HRESULT hr = m_piGerman.CreateInstance(__uuidof(LEMMATIZERLib::LemmatizerGerman));
		if (FAILED(hr))
		{
			AfxMessageBox(_T("Fatal Error: Lemmatizer creation failed"));
			return FALSE;
		}

		hr = m_piGermanAgram.CreateInstance(__uuidof(AGRAMTABLib::GerGramTab));
		if (FAILED(hr))
		{
			AfxMessageBox(_T("Fatal Error: German agramtab creation failed"));
			return FALSE;
		}
	//	Грузим словари
		m_bGerAgramLoaded = false;	
		m_bGermanLoaded = false;
		hr = m_piGerman->raw_LoadDictionariesRegistry();
		if (!FAILED(hr))
		{
			m_bGermanLoaded=true;
			hr = m_piGermanAgram->raw_Load();
			if (!FAILED(hr))
				m_bGerAgramLoaded=true;
		}
	//	Выставляем свойства
		m_piGerman->UseStatistic = FALSE;
	};



	if ( (Language == morphUnknown) || (Language == morphRussian))
	{
		HRESULT hr = m_piRussian.CreateInstance(__uuidof(LEMMATIZERLib::LemmatizerRussian));
		if (FAILED(hr))
		{
			AfxMessageBox(_T("Fatal Error: Lemmatizer creation failed"));
			return FALSE;
		}
		hr = m_piRussianAgram.CreateInstance(__uuidof(AGRAMTABLib::RusGramTab));
		if (FAILED(hr))
		{
			AfxMessageBox(_T("Fatal Error: Russian agramtab creation failed"));
			return FALSE;
		}
		m_bRusAgramLoaded = false;	
		m_bRussianLoaded = false;
		hr = m_piRussian->raw_LoadDictionariesRegistry();
		if (!FAILED(hr))
		{
			m_bRussianLoaded=true;
			hr = m_piRussianAgram->raw_Load();
			if (!FAILED(hr))
				m_bRusAgramLoaded=true;
		}
		m_piRussian->raw_LoadStatisticRegistry(LEMMATIZERLib::idlFinance);
		m_piRussian->UseStatistic = TRUE;
	};

//
	return TRUE;  // return TRUE  unless you std::set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDemoDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

AGRAMTABLib::IGramTabPtr CDemoDlg::GetGramTabByLanguage(int Lang)
{
	switch (Lang) {
		case morphRussian:  return m_bRusAgramLoaded ? m_piRussianAgram : 0;
		case morphEnglish:  return m_bEngAgramLoaded ? m_piEnglishAgram : 0;
		case morphGerman:  return m_bGerAgramLoaded ? m_piGermanAgram : 0;
	};
	return 0;
	assert (false);
};

LEMMATIZERLib::ILemmatizerPtr CDemoDlg::GetLemmatizerByLanguage(int Lang)
{
	switch (Lang) {
		case morphRussian:  return m_bRussianLoaded ? m_piRussian : 0;
		case morphEnglish:  return m_bEnglishLoaded ? m_piEnglish : 0;
		case morphGerman:  return m_bGermanLoaded ? m_piGerman : 0;
	};
	assert (false);
	return 0;
};

const char* CDemoDlg::GetStrByLanguage(int Lang)
{
	switch (Lang) {
		case morphRussian:  return "Rus";
		case morphEnglish:  return "Eng";
		case morphGerman:  return  "Ger";
	};
	assert (false);
	return 0;
};
inline bool check_language (std::string s, int Lang)
{
	switch (Lang) {
		case morphRussian:  return IsRussian(s);
		case morphEnglish:  return IsEnglish(s);
		case morphGerman:  return  IsGerman(s);
	};
	assert (false);
	return false;
};

inline MorphLanguageEnum InterfaceToEnum ( int Lang)
{
	switch (Lang) {
		case INLANG_RUSSIAN :  return morphRussian;
		case INLANG_ENGLISH :  return morphEnglish;
		case INLANG_GERMAN  :  return  morphGerman;
	};
	assert (false);
	return morphRussian;
};

void UnicoodeMakeUpper(CStringW& s) {
	std::wstring form = s;
	for (size_t i = 0; i < form.length(); ++i) {
		TCHAR b[2] = { form[i] , 0 };
		CharUpper(b);
		form[i] = b[0];
	}
	s = form.c_str();
}

void UnicoodeMakeLower(CStringW& s) {
	std::wstring form = s;
	for (size_t i = 0; i < form.length(); ++i) {
		TCHAR b[2] = { form[i] , 0 };
		CharLower(b);
		form[i] = b[0];
	}
	s = form.c_str();
}

std::wstring GetAccentedForms (LEMMATIZERLib::IParadigmPtr piParadigm, CString Form, MorphLanguageEnum langua)
{
	CString Result;
	UnicoodeMakeUpper(Form);
	for (size_t i=0; i<piParadigm->Count; i++)
	{
		CString s  = ReadStrFromCOM(piParadigm->Form[i]);
		if (s == Form)
		{
			BYTE AccentedCharNo = piParadigm->Accent[i];
			if (AccentedCharNo != 255)
			{
				UnicoodeMakeLower(s);
				CString c = s[AccentedCharNo];
				UnicoodeMakeUpper(c);
				s = s.Mid(0, AccentedCharNo) + c + s.Mid(AccentedCharNo + 1);
				Result +=  s;
				Result +=  ";";
			};
		};
	};

	return Result;
};

void CDemoDlg::OnButtonFind() 
{
	UpdateData();
	LEMMATIZERLib::ILemmatizerPtr			piLemmatizer;
	AGRAMTABLib::IGramTabPtr				piAgram;
	LEMMATIZERLib::IParadigmCollectionPtr	piParadigmCollection;
	LEMMATIZERLib::IParadigmPtr				piParadigm;

	if (m_strSrc.IsEmpty())
		return; 

	m_strSrc.Trim();

	CString src_str = m_strSrc;
	
	m_ctrlResults.DeleteAllItems();
	
	std::vector<MorphLanguageEnum> Languages;
	if (m_iLang == INLANG_ALL)
	{
		Languages.push_back(morphRussian);
		Languages.push_back(morphGerman);
		Languages.push_back(morphEnglish);
	}
	else
		Languages.push_back(InterfaceToEnum(m_iLang));


	// проверяем, что все src состоит  только из цифр, тогда запускаем CreateParadigmFromID
	size_t i=0;
	for (; i < src_str.GetLength(); i++)
		if (!iswdigit(src_str[i]))
			break;
	bool OnlyDigits = (i ==  src_str.GetLength());

	bool bFoundSomething = false;
	for (int LangNo=0; LangNo<Languages.size(); LangNo++)
	try
	{
		MorphLanguageEnum Lang = Languages[LangNo];
		if (!OnlyDigits) {
			auto utf8str = WstrToUtf8Str((const TCHAR*)src_str);
			try {
				if (!check_language(convert_from_utf8(utf8str.c_str(), Lang), Lang))
					continue;
			}
			catch (convert_exception e) {
				continue;
			}
		}
		piAgram = GetGramTabByLanguage(Lang);;
		piLemmatizer = GetLemmatizerByLanguage(Lang);;
		if (piLemmatizer == 0) continue;


		LEMMATIZERLib::IParadigmPtr ParadigmFromId = 0;
       	if (OnlyDigits)
			ParadigmFromId = piLemmatizer->CreateParadigmFromID(	_wtoi((const TCHAR*)src_str));
        else
		  piParadigmCollection = 
				piLemmatizer->CreateParadigmCollectionFromForm((const TCHAR*)src_str,
																iswupper(m_strSrc[0]), TRUE);


		CString result;
		
		
	//	Распечатываем парадигмы
		i=0;
		while (true)
		{
			if (OnlyDigits)
				piParadigm = ParadigmFromId;
			else
			{
			   if (i == piParadigmCollection->Count) break;
 			   piParadigm = piParadigmCollection->Item[i];
			};
			m_ctrlResults.InsertItem(i, UTF8toUTF16(GetStrByLanguage(Lang)));
			m_ctrlResults.SetItemText(i, COLUMN_STATUS, piParadigm->Founded ? _T("Found"): _T("Predicted"));
			bFoundSomething |= (piParadigm->Founded == TRUE);
			if (piParadigm->Founded)
			{
				CString strID;

                strID.Format(_T("%u"), piParadigm->ParadigmID);
				m_ctrlResults.SetItemText(i, COLUMN_ID, strID);
				try {
				  strID.Format(_T("%d"), piParadigm->HomonymWeight);
				  m_ctrlResults.SetItemText(i, COLUMN_HOMO, strID);
				} 
				catch (...){
					m_ctrlResults.SetItemText(i, COLUMN_HOMO, _T("not found"));
				};
				strID.Format(_T("%d"), piParadigm->WordWeight);
				m_ctrlResults.SetItemText(i, COLUMN_WORD, strID);
				m_ctrlResults.SetItemText(i, COLUMN_ACCENTS, GetAccentedForms(piParadigm, src_str, Lang).c_str() );

			}
			else
			{
				result = ReadStrFromCOM(piParadigm->SrcNorm);
				m_ctrlResults.SetItemText(i, COLUMN_ID, result);
				m_ctrlResults.SetItemText(i, COLUMN_HOMO, _T(""));
				m_ctrlResults.SetItemText(i, COLUMN_WORD, _T(""));
			}

			{
				std::string type_ancode = (const char*)piParadigm->TypeAncode;
				if (!type_ancode.empty())
				{
					QWORD grammems = piAgram->GetGrammems(type_ancode.c_str());
					result = ReadStrFromCOM(piAgram->GrammemsToStr(grammems));
					m_ctrlResults.SetItemText(i, COLUMN_TYPE, result);
				}
			}
			
			m_ctrlResults.SetItemText(i, COLUMN_NORM, ReadStrFromCOM(piParadigm->Norm));
			try
			{
				result = ReadStrFromCOM(piParadigm->SrcAncode);
			} catch(...)
			{
				result = ReadStrFromCOM(piParadigm->GetAncode(0));
			}
			m_ctrlResults.SetItemText(i, COLUMN_ANCODE, result);
			if (piAgram != NULL)
			{
				CString srccode = result;
				result = "";
				CString oldpart = "";
				for (int j = 0; j < srccode.GetLength(); j+=2)
				{
					std::wstring onecode = srccode.Mid(j, 2);
					BYTE POS = piAgram->GetPartOfSpeech(WstrToUtf8Str(onecode).c_str());
					auto newpart = ReadStrFromCOM (piAgram->GetPartOfSpeechStr(POS));
					if (oldpart != newpart)
					{
						oldpart = newpart;
						result += oldpart + " ";
					}
					QWORD grammems = piAgram->GetGrammems(onecode.c_str());
					CString out_grammems = ReadStrFromCOM(piAgram->GrammemsToStr(grammems));
					if (out_grammems.GetLength() > 0)
						out_grammems += "; ";
					result += out_grammems;
				}	
			}
			m_ctrlResults.SetItemText(i, COLUMN_FORM, result );
			if (OnlyDigits) break;
			i++;
		}
	}
	catch (...)
	{
	}

	if (bFoundSomething)
		for (i=0; i<m_ctrlResults.GetItemCount(); i++)
		if (m_ctrlResults.GetItemText(i, COLUMN_STATUS) == "Predicted")
			m_ctrlResults.DeleteItem(i--);

}

void CDemoDlg::OnOK() 
{
	m_piRussian = NULL;
	m_piEnglish = NULL;
	m_piRussianAgram = NULL;
	m_piEnglishAgram = NULL;
	CDialog::OnOK();
}

BOOL CDemoDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (m_pToolTip)
		m_pToolTip->RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}

int CDemoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CSplashWnd::ShowSplashScreen(this);

	return 0;
}

void CDemoDlg::OnRadioLiterature() 
{
	m_piRussian->raw_LoadStatisticRegistry(LEMMATIZERLib::idlLiterature);
	m_piEnglish->raw_LoadStatisticRegistry(LEMMATIZERLib::idlLiterature);
}

void CDemoDlg::OnRadioFinance() 
{
	m_piRussian->raw_LoadStatisticRegistry(LEMMATIZERLib::idlFinance);
	m_piEnglish->raw_LoadStatisticRegistry(LEMMATIZERLib::idlFinance);
}

void CDemoDlg::OnRadioComputer() 
{
	m_piRussian->raw_LoadStatisticRegistry(LEMMATIZERLib::idlComputer);
	m_piEnglish->raw_LoadStatisticRegistry(LEMMATIZERLib::idlComputer);
}
