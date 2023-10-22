// demoDlg.cpp : implementation file
//

#include "StdAfx.h"
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
	m_piRussian.LoadLemmatizer(morphRussian);
	m_piRussian.m_pLemmatizer->LoadStatisticRegistry(subjFinance);
	m_piRussian.m_pLemmatizer->m_bUseStatistic = true;
	m_piEnglish.LoadLemmatizer(morphEnglish);
	m_piGerman.LoadLemmatizer(morphGerman);

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

CMorphanHolder&  CDemoDlg::GetHolder(MorphLanguageEnum l) {
	switch (l) {
		case morphRussian:  return m_piRussian;
		case morphEnglish:  return m_piEnglish;
		case morphGerman:  return m_piGerman;
	};
	throw CExpc("Unknown lamguage");
	
}

const CAgramtab* CDemoDlg::GetGramTabByLanguage(MorphLanguageEnum l)
{
	return GetHolder(l).m_pGramTab;
};

const CLemmatizer* CDemoDlg::GetLemmatizerByLanguage(MorphLanguageEnum l)
{
	return GetHolder(l).m_pLemmatizer;
};

CString GetStrByLanguage(MorphLanguageEnum l)
{

	switch (l) {
		case morphRussian:  return "Rus";
		case morphEnglish:  return "Eng";
		case morphGerman:  return  "Ger";
	};
	assert (false);
	return "";
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

CString BuildCString(MorphLanguageEnum langua, std::string s)
{
	return utf8_to_utf16(convert_to_utf8(s, langua)).c_str();
}


CString GetAccentedForms (const CFormInfo& piParadigm, CString Form, MorphLanguageEnum langua)
{
	CString Result;
	UnicoodeMakeUpper(Form);
	for (size_t i=0; i<piParadigm.GetCount(); i++)
	{
		CString s  = BuildCString(langua, piParadigm.GetWordForm(i));
		if (s == Form)
		{
			BYTE AccentedCharNo = piParadigm.GetAccentedVowel(i);
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

bool CDemoDlg::BuildParadigmsForOneLanguage(MorphLanguageEnum langua, CString src_str, bool OnlyDigits) {
	auto piAgram = GetGramTabByLanguage(langua);
	auto piLemmatizer = GetLemmatizerByLanguage(langua);
	if (piLemmatizer == 0) return false;

	std::vector<CFormInfo> paradigms;
	if (OnlyDigits) {
		CFormInfo p;
		uint32_t id = _wtoi((const TCHAR*)src_str);
		if (piLemmatizer->CreateParadigmFromID(id, p)) {
			paradigms.push_back(p);
		}
	}
	else {
		std::string utf8_str = WstrToUtf8Str(src_str);
		std::string rml_str = convert_from_utf8(utf8_str.c_str(), langua);
		piLemmatizer->CreateParadigmCollection(false, rml_str, iswupper(src_str[0]), true, paradigms);
	}


	CString result;

	bool bFoundSomething = false;
	//	Распечатываем парадигмы
	for (size_t i = 0; i < paradigms.size(); ++i) {
		auto& paradigm = paradigms[i];
		m_ctrlResults.InsertItem((int)i, GetStrByLanguage(langua));
		m_ctrlResults.SetItemText(i, COLUMN_STATUS, paradigm.m_bFound ? _T("Found") : _T("Predicted"));
		bFoundSomething |= paradigm.m_bFound;
		if (paradigm.m_bFound)
		{
			CString strID;

			strID.Format(_T("%u"), paradigm.GetParadigmId());
			m_ctrlResults.SetItemText(i, COLUMN_ID, strID);
			try {
				strID.Format(_T("%d"), paradigm.GetHomonymWeight());
				m_ctrlResults.SetItemText(i, COLUMN_HOMO, strID);
			}
			catch (...) {
				m_ctrlResults.SetItemText(i, COLUMN_HOMO, _T("not found"));
			};
			strID.Format(_T("%d"), paradigm.GetWordWeight());
			m_ctrlResults.SetItemText(i, COLUMN_WORD, strID);
			m_ctrlResults.SetItemText(i, COLUMN_ACCENTS, GetAccentedForms(paradigm, src_str, langua));

		}
		else
		{
			result = BuildCString(langua, paradigm.GetSrcNorm());
			m_ctrlResults.SetItemText(i, COLUMN_ID, result);
			m_ctrlResults.SetItemText(i, COLUMN_HOMO, _T(""));
			m_ctrlResults.SetItemText(i, COLUMN_WORD, _T(""));
		}

		{
			std::string type_ancode = paradigm.GetCommonAncode();
			if (!type_ancode.empty())
			{
				grammems_mask_t grammems;
				piAgram->GetGrammems(type_ancode.c_str(), grammems);
				result = BuildCString(langua, piAgram->GrammemsToStr(grammems));
				m_ctrlResults.SetItemText(i, COLUMN_TYPE, result);
			}
		}

		m_ctrlResults.SetItemText(i, COLUMN_NORM, BuildCString(langua, paradigm.GetWordForm(0)));
		try
		{
			result = BuildCString(langua, paradigm.GetSrcAncode());
		}
		catch (...)
		{
			result = BuildCString(langua, paradigm.GetAncode(0));
		}
		m_ctrlResults.SetItemText(i, COLUMN_ANCODE, result);

		result = BuildCString(langua, paradigm.GetGramInfoStr(piAgram));
		m_ctrlResults.SetItemText(i, COLUMN_FORM, result);
	}
	return bFoundSomething;
}

void CDemoDlg::OnButtonFind() 
{
	UpdateData();

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
	for (auto langua: Languages)
	try
	{
		if (!OnlyDigits) {
			auto utf8str = WstrToUtf8Str((const TCHAR*)src_str);
			try {
				if (!check_language(convert_from_utf8(utf8str.c_str(), langua), langua))
					continue;
			}
			catch (convert_exception e) {
				continue;
			}
		}
		if (BuildParadigmsForOneLanguage(langua, src_str, OnlyDigits)) {
			bFoundSomething = true;
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
	m_piRussian.m_pLemmatizer->LoadStatisticRegistry(subjLiterature);
	m_piEnglish.m_pLemmatizer->LoadStatisticRegistry(subjLiterature);
}

void CDemoDlg::OnRadioFinance() 
{
	m_piRussian.m_pLemmatizer->LoadStatisticRegistry(subjFinance);
	m_piEnglish.m_pLemmatizer->LoadStatisticRegistry(subjFinance);
}

void CDemoDlg::OnRadioComputer() 
{
	m_piRussian.m_pLemmatizer->LoadStatisticRegistry(subjComputer);
	m_piEnglish.m_pLemmatizer->LoadStatisticRegistry(subjComputer);
}
