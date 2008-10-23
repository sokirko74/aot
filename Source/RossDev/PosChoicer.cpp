// PosChoicer.cpp : implementation file
//

#include "stdafx.h"
#include "..\rossdev\resource.h"
#include "PosChoicer.h"
#include "../common/utilit.h"
#include "NewArticleWizard.h"
#include "NewRefArticles.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPosChoicer dialog

IMPLEMENT_DYNCREATE(CPosChoicer, CPropertyPage)

CPosChoicer::CPosChoicer() : CPropertyPage(CPosChoicer::IDD)
{
	 
}
 


void CPosChoicer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPosChoicer)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPosChoicer, CDialog)
	//{{AFX_MSG_MAP(CPosChoicer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPosChoicer message handlers

BOOL CPosChoicer::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here*/
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CPosChoicer::OnWizardNext()
{
	CNewArticleWizard* P  = (CNewArticleWizard*)GetParent();
	P->PartOfSpeech = -1;
	m_ResultString = "";

	int  pos = m_List.GetCurSel();
	if  (    (pos == LB_ERR)
		|| (m_List.GetCount() == 0)
		|| (pos == 0)
		)
	{ //::MessageBox(0, "В списке не выделено ни одного словарного входа", "Message Box", MB_OK);
		return 0;};


		m_List.GetText( pos, m_ResultString );
		if (m_ResultString.IsEmpty())
		{ //::MessageBox(0, "В списке не выделено ни одного словарного входа", "Message Box", MB_OK);
			return 0;};

			m_ResultString = m_ResultString + CString ('\n');

			P->PartOfSpeech = m_List.GetItemData( pos );

			CRossDevApp* A = ((CRossDevApp*)AfxGetApp());

			// проверка вида глагола. Если  глагол несовершенного вида, то нужно поискать его видовой вариант
			// совершенного вида и создать для выбранных из них отсылочные статьи.
			if (P->GetPageCount() == 4) 
			{
				delete P->GetPage(3);
				P->RemovePage(3);
			};


			if (P->PartOfSpeech == INFINITIVE) 
				if (!m_ParadigmCollection.empty())
					{
						vector<LONG> Perfs;
						for (size_t i = 0; i < m_ParadigmCollection.size(); i++)
						{
							vector<DWORD> ResVector;
							GetSemBuilder().m_RusStr.m_pData->m_AspDict.nonperf2perf(m_ParadigmCollection[i].GetParadigmId(), ResVector);
							for(size_t k = 0; k < ResVector.size(); k++)
								Perfs.push_back(ResVector[k]);
						};
						if (Perfs.size() > 0)
						{
							CNewRefArticles* p;
							if (P->GetPageCount() < 4)
							{
								p = new  CNewRefArticles("Создание отсылочных статей для глаголов несов. вида");
								P->AddPage(p);
							}
							else
								p =  (CNewRefArticles*)P->GetPage(3);

							p->SetRefs(Perfs);
						};

					};

			return 0;

};

BOOL CPosChoicer::OnSetActive() 
{
	CNewArticleWizard* P  = (CNewArticleWizard*)GetParent();
	P->SetWizardButtons( PSWIZB_BACK|PSWIZB_NEXT);
	const CRusGramTab* A = GetSemBuilder().m_RusStr.m_pData->GetRusGramTab();
	m_List.ResetContent();
	m_List.AddString("other");

	for (int i =0; i<m_ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = m_ParadigmCollection[i];
		CString S((char *)Paradigm.GetSrcAncode().c_str());
		if (S.GetLength() < 2)  throw CExpc("Bad morphology format");
		size_t PartOfSpeech = A->GetPartOfSpeech((const char*) S);

		CString LineToAdd;
		switch  (PartOfSpeech) 
		{

		case NOUN:			LineToAdd = "СУЩ:ИГ"; break;
		case INFINITIVE:	LineToAdd = "ГЛ:ГГ"; break;
		case ADJ_FULL:		LineToAdd = "ПРИЛ:с_опр"; break;
		case ADV:			LineToAdd = "НАР:ОБСТ_ГР"; break;
		case PREDK:			LineToAdd = "НАР:нар_опр"; break;
		};


		if (!LineToAdd.IsEmpty())
		{
			LineToAdd = CString(P->m_pRossDoc->GetGramFetFieldName())+ " = 1 "+LineToAdd;
			int pos = m_List.AddString(LineToAdd);
			if (pos >= 0) m_List.SetItemData( pos , PartOfSpeech);
		};



	};

	if (m_List.GetCount() > 0)
	{
		m_List.SetCurSel(0);
		m_List.SetSel(0);
	}


	return TRUE;
};
