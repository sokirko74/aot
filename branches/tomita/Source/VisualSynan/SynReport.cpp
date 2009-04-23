#include "SynReport.h"


bool GlobalOpenReport(CString S, CString Name)
{
	CDocTemplate* T = GetReportTemplate();
	CReportDoc* pDocument = (CReportDoc*)T->CreateNewDocument();
    ASSERT_VALID(pDocument);
	
	
	T->InitialUpdateFrame(T->CreateNewFrame(pDocument, NULL), pDocument, TRUE);
	pDocument->m_bRTF = false;
	CRichEditCtrl& C =  pDocument->GetView()->GetRichEditCtrl();
	pDocument->SetPathName(Name, FALSE);
		
    C.SetWindowText (S);
	pDocument->InitFonts();
	return true;
};

