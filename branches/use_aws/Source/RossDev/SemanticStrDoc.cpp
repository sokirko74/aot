// graphtest3Doc.cpp : implementation of the CSemanticStrDoc class
//

#include "stdafx.h"
#include "../common/utilit.h"
#include "SemanticStrDoc.h"
#include "SemanticStrView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSemanticStrDoc

IMPLEMENT_DYNCREATE(CSemanticStrDoc, CDocument)

BEGIN_MESSAGE_MAP(CSemanticStrDoc, CDocument)
	//{{AFX_MSG_MAP(CSemanticStrDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSemanticStrDoc construction/destruction




BOOL CSemanticStrDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSemanticStrDoc serialization





extern Tcl_Interp* theInterp;



/////////////////////////////////////////////////////////////////////////////
// CSemanticStrDoc diagnostics

#ifdef _DEBUG
void CSemanticStrDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSemanticStrDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSemanticStrDoc commands




void CSemanticStrDoc::SetTitle(LPCTSTR lpszTitle) 
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::SetTitle("Семантический анализ");
}
