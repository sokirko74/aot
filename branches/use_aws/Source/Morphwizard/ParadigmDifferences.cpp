// CParadigmDifferences.cpp : implementation file
//

#include "stdafx.h"
#include "Morphwizard.h"
#include "ParadigmDifferences.h"
#include "SLFDocument.h"

// CParadigmDifferences

IMPLEMENT_DYNCREATE(CParadigmDifferences, CSizeFormView)

CParadigmDifferences::CParadigmDifferences()
	: CSizeFormView(CParadigmDifferences::IDD)
	, m_Results(_T(""))
	, m_Paradigm1(-1)
	, m_Paradigm2(-1)
{
}

CParadigmDifferences::~CParadigmDifferences()
{
}

void CParadigmDifferences::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RESULTS, m_Results);
	DDX_Text(pDX, IDC_PARAD_1, m_Paradigm1);
	DDX_Text(pDX, IDC_PARAD_2, m_Paradigm2);
}

BEGIN_MESSAGE_MAP(CParadigmDifferences, CSizeFormView)
	ON_BN_CLICKED(ID_ANALYSE, OnBnClickedAnalyse)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CParadigmDifferences diagnostics

#ifdef _DEBUG
void CParadigmDifferences::AssertValid() const
{
	CFormView::AssertValid();
}

void CParadigmDifferences::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CParadigmDifferences message handlers

void CParadigmDifferences::OnBnClickedAnalyse()
{
	UpdateData(TRUE);
	if (m_Paradigm1 >= GetWizard()->m_FlexiaModels.size())
	{
			ErrorMessage(Format("Paradigm no %i is not found",m_Paradigm1));
			return;
	}
	if (m_Paradigm2 >= GetWizard()->m_FlexiaModels.size())
	{
			ErrorMessage(Format("Paradigm no %i is not found",m_Paradigm2));
			return;
	}
	std::string s = GetWizard()->show_differences_in_two_paradigms(m_Paradigm1,m_Paradigm2);
	m_Results = s.c_str();
	UpdateData(FALSE);
}


CSLFDocument* CParadigmDifferences::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSLFDocument)));
	return (CSLFDocument*)m_pDocument;
};

MorphoWizard* CParadigmDifferences::GetWizard()
{
	return  GetDocument()->GetWizard();
};

void CParadigmDifferences::OnBnClickedCancel()
{
	OnClose();
}

void CParadigmDifferences::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CSizeFormView::OnActivate(nState, pWndOther, bMinimized);
	SetInputLanguage(GetWizard()->m_Language);
}
