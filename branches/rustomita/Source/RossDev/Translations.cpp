// Translations.cpp : implementation file
//

#include "stdafx.h"
#include "..\rossdev\resource.h"
#include "Translations.h"
#include "NewArticleWizard.h"
#include  "../BinaryDictsLib/SetOfWordPairs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranslations property page

IMPLEMENT_DYNCREATE(CTranslations, CPropertyPage)

CTranslations::CTranslations() : CPropertyPage(CTranslations::IDD)
{
	//{{AFX_DATA_INIT(CTranslations)
		// NOTE: the ClassWizard will add member initialization here
	m_ResultString = "";
	//}}AFX_DATA_INIT
}


void CTranslations::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranslations)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTranslations, CPropertyPage)
	//{{AFX_MSG_MAP(CTranslations)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranslations message handlers

BOOL CTranslations::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTranslations::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	CNewArticleWizard* P  = (CNewArticleWizard*)GetParent();
	if (P->GetPageCount() == 3)
		P->SetWizardButtons( PSWIZB_BACK|PSWIZB_NEXT|PSWIZB_FINISH);
	else
		P->SetWizardButtons( PSWIZB_BACK|PSWIZB_NEXT);

	
	const CBinaryDictionary& Dict = GetSemBuilder().m_RusStr.m_pData->m_BinaryDict; 
	m_List.ResetContent();
	const CRusGramTab* A = GetSemBuilder().m_RusStr.m_pData->GetRusGramTab();
	RusLemmas.clear();

	int flag_count;

	for (int i =0; i<m_ParadigmCollection.size(); i++)
	{
		const CFormInfo& FormInfo = m_ParadigmCollection[i];
		if (FormInfo.m_bFound)
		{
			BYTE pos = A->GetPartOfSpeech(FormInfo.GetSrcAncode().c_str());
			RusLemmas.push_back (TransLemma(FormInfo.GetParadigmId(),pos));
		}
	};

	flag_count = Dict.GetFlagCount();

	for (size_t i = 0; i <  RusLemmas.size(); i++)
	{
		if (P->PartOfSpeech == RusLemmas[i].PartOfSpeech)
		{
			CSetOfWordPairs Pairs = Dict.TranslateIndirect(RusLemmas[i].ParadigmId);

			for (size_t k = 0;  k < Pairs.get_Count(); k++)
			{
				DWORD Id =  Pairs.GetId(k);
				CFormInfo Prd;
				bool bRes = GetSemBuilder().m_RusStr.m_pData->GetEngLemmatizer()->CreateParadigmFromID(Id, Prd);
				if (!bRes) 
				{
					ErrorMessage("Bad paradigm id");
					continue;
				}

				CString Q = Prd.GetWordForm(0).c_str();

				//!!! получаем флаг
				string t;
				int POS = Pairs.GetFlag(k, 0, t);
				if (   ((P->PartOfSpeech == NOUN) &&  (POS != 0 ))
					|| ((P->PartOfSpeech == INFINITIVE) &&  (POS != 1 ))
					|| ((P->PartOfSpeech == ADJ_FULL) &&  (POS != 2 ))
					|| ((P->PartOfSpeech == ADV) &&  (POS != 3 )))
					continue;


				for(int fl = 1; fl < flag_count; fl++)
				{
					Pairs.GetFlag(k, fl, t);
					if (t.length() > 0)
						Q +=  CString (" ") + CString(t.c_str());
				}
				m_List.AddString(Q);
			};
		};
	};




	return CPropertyPage::OnSetActive();
}

BOOL CTranslations::OnWizardFinish()
{ 
  OnWizardNext();
  EndDialog( IDOK );
  return TRUE;
};

LRESULT CTranslations::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
   m_ResultString = "";
   for (size_t i=0; i < m_List.GetCount(); i++)
	  if (m_List.GetSel( i ) )
	  {
		  CString Q;
		  m_List.GetText(i, Q);
		  CString T = Q.SpanExcluding(" ");
		  T.MakeLower();
		  m_ResultString +=   T + CString ("\n");;

	  };

   if (!m_ResultString .IsEmpty())
   {
	  const CNewArticleWizard* Wzd  = (CNewArticleWizard*)GetParent();
	  m_ResultString =  CString(Wzd->m_pRossDoc->GetEnglishFieldName())+ " = " + m_ResultString;
   };

   return CPropertyPage::OnWizardNext();
}
