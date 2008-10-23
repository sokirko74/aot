// CntrItem.cpp : implementation of the CTestCntrItem class
//

#include "stdafx.h"

#include "ReportDoc.h"
//#include "testView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestCntrItem implementation

IMPLEMENT_SERIAL(CTestCntrItem, CRichEditCntrItem, 0)

CTestCntrItem::CTestCntrItem(REOBJECT* preo, CReportDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
	
}

CTestCntrItem::~CTestCntrItem()
{
	// TODO: add cleanup code here
	
}

/////////////////////////////////////////////////////////////////////////////
// CTestCntrItem diagnostics

#ifdef _DEBUG
void CTestCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CTestCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
