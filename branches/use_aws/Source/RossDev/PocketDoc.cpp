// ArticleDoc.cpp : implementation of the CPocketDoc class
//

#include "stdafx.h"
#include "PocketDoc.h"
#include "../common/utilit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPocketDoc

IMPLEMENT_DYNCREATE(CPocketDoc, CDocument)

BEGIN_MESSAGE_MAP(CPocketDoc, CDocument)
	//{{AFX_MSG_MAP(CPocketDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CPocketDoc construction/destruction

CPocketDoc::CPocketDoc()
{
}

CPocketDoc::~CPocketDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPocketDoc diagnostics

#ifdef _DEBUG
void CPocketDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPocketDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPocketDoc commands


