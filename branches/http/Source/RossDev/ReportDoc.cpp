// ArticleDoc.cpp : implementation of the CReportDoc class
//

#include "stdafx.h"
#include "ReportDoc.h"
#include "../common/utilit.h"
#include "CntrItem.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CReportDoc

IMPLEMENT_DYNCREATE(CReportDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CReportDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CReportDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CReportDoc construction/destruction



CReportDoc::CReportDoc()
{
}

CReportDoc::~CReportDoc()
{
}

void CReportDoc::InitFonts()
{
	CRichEditCtrl& C =  GetView()->GetRichEditCtrl();
	CString  S;
	C.GetWindowText(S);
	C.SetReadOnly(TRUE);
	SetModifiedFlag(FALSE);

	int l = C.LineLength(0);
	C.SetSel (0, l);
	CHARFORMAT cf;
	C.GetSelectionCharFormat (cf);
	cf.yHeight = 300;
	strcpy (cf.szFaceName,"Times New Roman");
	cf.bCharSet = RUSSIAN_CHARSET;
	C.SetSelectionCharFormat (cf);

	C.SetSel (l+1, S.GetLength());
	C.GetSelectionCharFormat (cf);
	strcpy (cf.szFaceName,"FixedSys");
	cf.bCharSet = RUSSIAN_CHARSET;
	C.SetSelectionCharFormat (cf);
	C.SetSel (0, 0);
    C.Invalidate();
	C.SetReadOnly();
	C.SetModify(FALSE);


};


void CReportDoc::Serialize(CArchive& ar)   // overridden for document i/o
{
	if (ar.IsStoring())
	{
		CRichEditDoc::Serialize(ar);
	}
	else
	{
		CString S;
        CString Report = "";
		while (ar.ReadString(S))
		{
		  Report += S +  CString ("\r\n");
		};
		GetView()->GetRichEditCtrl().SetWindowText(Report);
		InitFonts ();
	};
};

CRichEditCntrItem* CReportDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return new CTestCntrItem(preo, (CReportDoc*) this);
}


/////////////////////////////////////////////////////////////////////////////
// CReportDoc diagnostics

#ifdef _DEBUG
void CReportDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CReportDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportDoc commands



