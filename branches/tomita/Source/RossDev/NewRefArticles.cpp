// NewRefArticles.cpp : implementation file
//

#include "stdafx.h"
#include "..\rossdev\resource.h"
#include "NewRefArticles.h"
#include "NewArticleWizard.h"
#include "NewTextEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE	
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewRefArticles property page

IMPLEMENT_DYNCREATE(CNewRefArticles, CPropertyPage)

CNewRefArticles::CNewRefArticles() : CPropertyPage(CNewRefArticles::IDD)
{
};
CNewRefArticles::CNewRefArticles(CString Caption) : CPropertyPage(CNewRefArticles::IDD)
{
	//{{AFX_DATA_INIT(CNewRefArticles)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
   m_Caption = Caption;
}

CNewRefArticles::~CNewRefArticles()
{
}

void CNewRefArticles::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewRefArticles)
	DDX_Control(pDX, IDC_LIST1, m_List);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewRefArticles, CPropertyPage)
	//{{AFX_MSG_MAP(CNewRefArticles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewRefArticles message handlers

void CNewRefArticles::SetRefs(vector<LONG>& RefsId)
{
	m_RefsId.clear();
	for (size_t i=0; i<RefsId.size(); i++)
     m_RefsId.push_back(RefsId[i]);

}

BOOL CNewRefArticles::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	SetWindowText (m_Caption);
	CRossDevApp* App = (CRossDevApp*)AfxGetApp();
	CNewArticleWizard* P  = (CNewArticleWizard*)GetParent();

    m_List.ResetContent();
	P->SetWizardButtons( PSWIZB_BACK|PSWIZB_NEXT|PSWIZB_FINISH);

	for (size_t k = 0;  k < m_RefsId.size(); k++)
	{
		CFormInfo Prd;
		if (GetSemBuilder().m_RusStr.m_pData->GetRusLemmatizer()->CreateParadigmFromID(m_RefsId[k], Prd))
			m_List.AddString(Prd.GetWordForm(0).c_str());
	};
	return CPropertyPage::OnSetActive();
}

BOOL CNewRefArticles::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
   OnWizardNext();
   EndDialog( IDOK );
   return TRUE;
}

BOOL CNewRefArticles::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText (m_Caption);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CNewRefArticles::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_ResultString = "";
	CNewArticleWizard* Wzd  = (CNewArticleWizard*)GetParent();
	CString MeanNum =  ((CNewTextEntry*)Wzd->GetPage(0))->m_MeanNum;
	CString Lemma =  ((CNewTextEntry*)Wzd->GetPage(0))->m_UnitStr;

    for (size_t i=0; i < m_List.GetCount(); i++)
	if (m_List.GetSel( i ) )
	 {
		  CString Q;
		  m_List.GetText(i, Q);
		  Q.MakeLower();
		  m_ResultString +=   Lemma + CString(" . # ")+ MeanNum + CString ("\n");;
          m_Lemma = Q;
		  break;
	 };

    if (!m_ResultString.IsEmpty())
	{
		m_ResultString =  CString(Wzd->m_pRossDoc->GetReferenceFieldName())+ " = " + m_ResultString;
	};

   return CPropertyPage::OnWizardNext();
}
