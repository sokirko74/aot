#include "stdafx.h"
#include "Morphwizard.h"
#include "AccentModelDiff.h"
#include "SLFDocument.h"

// CAccentModelDiff

IMPLEMENT_DYNCREATE(CAccentModelDiff, CFormView)

CAccentModelDiff::CAccentModelDiff()
	: CFormView(CAccentModelDiff::IDD)
	, m_Results(_T(""))
	, m_AccentModel1(-1)
	, m_AccentModel2(-1)
{
}

CAccentModelDiff::~CAccentModelDiff()
{
}

void CAccentModelDiff::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DIFF_RESULTS, m_Results);
	DDX_Text(pDX, IDC_ACCENT_MODEL_1, m_AccentModel1);
	DDX_Text(pDX, IDC_ACCENT_MODEL_2, m_AccentModel2);
}

BEGIN_MESSAGE_MAP(CAccentModelDiff, CFormView)
	ON_BN_CLICKED(ID_ANALYSE_ACCENT_DIFF, OnBnClickedAnalyse)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CAccentModelDiff diagnostics

#ifdef _DEBUG
void CAccentModelDiff::AssertValid() const
{
	CFormView::AssertValid();
}

void CAccentModelDiff::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


void CAccentModelDiff::OnBnClickedAnalyse()
{
	UpdateData(TRUE);
	if (m_AccentModel1 >= GetWizard()->m_AccentModels.size())
	{
		ErrorMessage(Format("Accent Model no %i is not found", m_AccentModel1));
		return;
	}
	if (m_AccentModel2 >= GetWizard()->m_AccentModels.size())
	{
		ErrorMessage(Format("Accent Model %i is not found", m_AccentModel1));
		return;
	}
	size_t c1 = GetWizard()->m_AccentModels[m_AccentModel1].m_Accents.size();
	size_t c2 = GetWizard()->m_AccentModels[m_AccentModel2].m_Accents.size();
	if (c1 != c2) {
		ErrorMessage(Format("Accent Model %i has is %i items, accent Model %i has is %i items. Cannot compare them.", 
			m_AccentModel1, c1, m_AccentModel2, c2));
		return;
	}
	std::string base = _R("мамамама");
	int flexia_model_no = -1;
	for (auto const& p : GetWizard()->m_LemmaToParadigm) {
		// search a real word
		if (p.second.m_AccentModelNo == m_AccentModel1 || p.second.m_AccentModelNo == m_AccentModel2) {
			base = p.first;
			flexia_model_no = p.second.m_FlexiaModelNo;
			break;
		}
	}
	if (flexia_model_no == -1) {
		ErrorMessage(Format("Accent Models %i and %i are registered, but there is no word of these model",
			m_AccentModel1, m_AccentModel2));
		return;
	}
	const CFlexiaModel& flexia_model = GetWizard()->m_FlexiaModels[flexia_model_no];
	CString s1 = GetWizard()->FromRMLEncoding(GetWizard()->mrd_to_slf(base, flexia_model, m_AccentModel1, UnknownAccent, 79)).c_str();
	CString s2 = GetWizard()->FromRMLEncoding(GetWizard()->mrd_to_slf(base, flexia_model, m_AccentModel2, UnknownAccent, 79)).c_str();
	auto lines1 = StringSplit(s1, "\r\n");
	auto lines2 = StringSplit(s2, "\r\n");
	m_Results = "Different accents:\r\n";
	if (lines1.size() != lines2.size()) {
		ErrorMessage("Failed");
		return;
	}
	for (size_t i = 0; i < lines1.size(); ++i) {
		if (lines1[i] != lines2[i]) {
			CString s;
			s.Format(_T("%s != %s\r\n"), lines1[i], lines2[i]);
			m_Results += s;
		}
	}
	UpdateData(FALSE);
	
}


CSLFDocument* CAccentModelDiff::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSLFDocument)));
	return (CSLFDocument*)m_pDocument;
};

MorphoWizard* CAccentModelDiff::GetWizard()
{
	return  GetDocument()->GetWizard();
};

void CAccentModelDiff::OnBnClickedCancel()
{
	OnClose();
}

void CAccentModelDiff::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFormView::OnActivate(nState, pWndOther, bMinimized);
}
