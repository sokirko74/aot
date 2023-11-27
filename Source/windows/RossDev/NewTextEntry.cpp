// NewTextEntry.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
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
   BYTE MeanNum = _ttoi (m_MeanNum);
   if (m_UnitStr.IsEmpty())
   {
	   AfxMessageBox (_T("The text entry cannot be empty"));
	   return -1;
   };
   auto unit_str_u8 = utf16_to_utf8((const TCHAR*)m_UnitStr);
   uint16_t UnitNo = GetRoss()->LocateUnit (unit_str_u8.c_str(), MeanNum);
   if (UnitNo  != ErrUnitNo)
   {
	   if (::MessageBox(0, _T("An entry with the same name already exists. Add a new sense?"), _T("Message Box"), MB_OKCANCEL) != IDOK)
		 return -1;
	   int k = UnitNo+1;
	   for (; k < GetRoss()->GetUnitsSize(); k++)
		   if (unit_str_u8 != GetRoss()->GetEntryStr(k))
			   break;
        MeanNum =  GetRoss()->GetUnitMeanNum(k-1) + 1;
	   
   };
   m_MeanNum.Format(_T("%i"), MeanNum);
   UpdateData(FALSE);
      //ищем слово в морфологии
   CRossDevApp* A = ((CRossDevApp*)AfxGetApp());
   ((CPosChoicer*)Wzd->GetPage(1))->m_ParadigmCollection.clear();
   ((CTranslations*)Wzd->GetPage(2))->m_ParadigmCollection.clear();
   std::vector<CFormInfo>& ParadigmCollection = ((CPosChoicer*)Wzd->GetPage(1))->m_ParadigmCollection;
   const CLemmatizer* P = GetSemBuilder().m_RusStr.m_pData->GetRusLemmatizer();
   if (P)
   {
		auto s8 = _R(unit_str_u8.c_str());
		P->CreateParadigmCollection(true, s8, false, false, ParadigmCollection);
 		((CTranslations*)Wzd->GetPage(2))->m_ParadigmCollection = ParadigmCollection;
	};

   
   return 0;
};
