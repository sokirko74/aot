// VisualSynanDoc.cpp : implementation of the CVisualSynanDoc class
//

#include "stdafx.h"
#include "VisualSynan.h"
#include "MainFrm.h"
#include "VisualSynanDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVisualSynanDoc

IMPLEMENT_DYNCREATE(CVisualSynanDoc, CDocument)

BEGIN_MESSAGE_MAP(CVisualSynanDoc, CDocument)
	//{{AFX_MSG_MAP(CVisualSynanDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualSynanDoc construction/destruction

CVisualSynanDoc::CVisualSynanDoc()
{
}

CVisualSynanDoc::~CVisualSynanDoc()
{
}

BOOL CVisualSynanDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CVisualSynanDoc serialization


void CVisualSynanDoc::PreCloseFrame( CFrameWnd* pFrame )
{
	CDocument::PreCloseFrame(pFrame);
}


/////////////////////////////////////////////////////////////////////////////
// CVisualSynanDoc diagnostics

#ifdef _DEBUG
void CVisualSynanDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVisualSynanDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVisualSynanDoc commands

static BOOL GetSentencesFromSynAn(CVisualSynanDoc& C, CString strText, BOOL bFile) 
{
	try {
		CTime StartTime = CTime::GetCurrentTime();
		CVisualSynanApp* A = (CVisualSynanApp*)AfxGetApp();
		BOOL bRes = A->m_SyntaxHolder.GetSentencesFromSynAn((const char*)strText, bFile, TRUE, TRUE);
		if( !bRes )
				return FALSE;
			
		bRes = C.m_VisualSentences.FillSentencesArray(A->m_SyntaxHolder.m_piSentCollection);		
		CTime EndTime = CTime::GetCurrentTime();
		CTimeSpan Span = EndTime - StartTime;
		C.m_WorkTimeStr = Span.Format("%M min %S sec");
		return bRes;
	}
	catch (...)
	{
		return FALSE;
	};

};


BOOL CVisualSynanDoc::ProcessString(CString strText) 
{
	return GetSentencesFromSynAn(*this, strText, FALSE);
}

BOOL CVisualSynanDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	((CMainFrame*)::AfxGetMainWnd())->m_bNewDoc = FALSE;
	CString strPath = lpszPathName;
	strPath.MakeLower();
	return GetSentencesFromSynAn(*this, strPath, TRUE);
}


BOOL CVisualSynanDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	SetModifiedFlag (FALSE);
	return true;
}

BOOL CVisualSynanDoc::SaveModified() 
{
	// TODO: Add your specialized code here and/or call the base class
	SetModifiedFlag (FALSE);
	return TRUE;
}
