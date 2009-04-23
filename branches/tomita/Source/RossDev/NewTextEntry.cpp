// NewTextEntry.cpp : implementation file
//

#include "stdafx.h"
#include "..\rossdev\resource.h"
#include "NewTextEntry.h"
#include "NewArticleWizard.h"
#include "PosChoicer.h"
#include "Translations.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewTextEntry dialog


CNewTextEntry::CNewTextEntry(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CNewTextEntry::IDD)
{
	//{{AFX_DATA_INIT(CNewTextEntry)
	m_UnitStr = _T("");
	m_MeanNum = _T("1");
	//}}AFX_DATA_INIT
}


void CNewTextEntry::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewTextEntry)
	DDX_Text(pDX, IDC_EDIT1, m_UnitStr);
	DDV_MaxChars(pDX, m_UnitStr, 100);
	DDX_Text(pDX, IDC_EDIT2, m_MeanNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewTextEntry, CPropertyPage)
	//{{AFX_MSG_MAP(CNewTextEntry)
		// NOTE: the ClassWizard will add message  macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewTextEntry message handlers


BOOL CNewTextEntry::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
    CNewArticleWizard* Wzd  = (CNewArticleWizard*)GetParent();

	Wzd->SetWizardButtons(PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}

LRESULT CNewTextEntry::OnWizardNext()
{
   UpdateData();
   CNewArticleWizard* Wzd  = (CNewArticleWizard*)GetParent();
   m_UnitStr.TrimLeft();
   m_UnitStr.TrimRight();
   BYTE MeanNum = atoi (m_MeanNum);
   if (m_UnitStr.IsEmpty())
   {
	   AfxMessageBox ("The text entry cannot be empty");
	   return -1;
   };
   WORD UnitNo = GetRoss()->LocateUnit ((const char*)m_UnitStr,MeanNum);
   if (UnitNo  != ErrUnitNo)
   {
	   if (::MessageBox(0, "An entry with the same name already exists. Add a new sense?", "Message Box", MB_OKCANCEL) != IDOK)
		 return -1;
	   int k = UnitNo+1;
	   for (; k < GetRoss()->GetUnitsSize(); k++)
		   if (strcmp (m_UnitStr, GetRoss()->GetEntryStr(k).c_str()))
			   break;
        MeanNum =  GetRoss()->GetUnitMeanNum(k-1)+1;
	   
   };
   m_MeanNum.Format("%i", MeanNum);
   UpdateData(FALSE);
      //ищем слово в морфологии
   CRossDevApp* A = ((CRossDevApp*)AfxGetApp());
   ((CPosChoicer*)Wzd->GetPage(1))->m_ParadigmCollection.clear();
   ((CTranslations*)Wzd->GetPage(2))->m_ParadigmCollection.clear();
   vector<CFormInfo>& ParadigmCollection = ((CPosChoicer*)Wzd->GetPage(1))->m_ParadigmCollection;

   
   const CLemmatizer* P = GetSemBuilder().m_RusStr.m_pData->GetRusLemmatizer();
   if (P)
   {
		
		P->CreateParadigmCollection(true, string((const char*)m_UnitStr), false, false, ParadigmCollection);
 		((CTranslations*)Wzd->GetPage(2))->m_ParadigmCollection = ParadigmCollection;
	};

   
   return 0;
};
