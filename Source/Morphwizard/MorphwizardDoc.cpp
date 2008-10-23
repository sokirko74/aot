// MorphwizardDoc.cpp : implementation of the CMorphwizardDoc class
//

#include "stdafx.h"
#include "Morphwizard.h"
#include "DlgLogin.h"
#include "GriIni.h"

#include "MorphwizardDoc.h"
#include "MorphwizardView.h"
#include "ProgressBar.h"

#include "SLFDocument.h"
#include "SLFView.h"


#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#endif


// CMorphwizardDoc

IMPLEMENT_DYNCREATE(CMorphwizardDoc, CDocument)

BEGIN_MESSAGE_MAP(CMorphwizardDoc, CDocument)
END_MESSAGE_MAP()


// CMorphwizardDoc construction/destruction

CMorphwizardDoc::CMorphwizardDoc()
{
	// TODO: add one-time construction code here

}

CMorphwizardDoc::~CMorphwizardDoc()
{
}

BOOL CMorphwizardDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


BOOL CMorphwizardDoc::DoFileSave()
{
	if (GetWizard()->m_ReadOnly)
	{
		AfxMessageBox ("Wizard is readonly, cannot be saved");
		return TRUE;
	};
	
	CWaitCursor wait;
	try
	{
		CWizardProgressMeter meter(m_Wizard);
		GetWizard()->save_mrd();
	}
	catch(CExpc e)
	{
		AfxMessageBox(e.m_strCause.c_str());
		return FALSE;
	}
	return TRUE;
}



// CMorphwizardDoc serialization

void CMorphwizardDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	   	// ask username
		CDlgLogin dlgLogin;
		if( dlgLogin.DoModal() != IDOK )
			throw CExpc("No login information");

		CGriIni cIni;
		cIni.Init();
		CWizardProgressMeter meter(m_Wizard);
		bool ret = GetWizard()->load_wizard(ar.GetFile()->GetFilePath(), dlgLogin.m_name);
		
		SetInputLanguage(GetWizard()->m_Language);

		cIni.Exit();
	}
}


// CMorphwizardDoc diagnostics

#ifdef _DEBUG
void CMorphwizardDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMorphwizardDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

//----------------------------------------------------------------------------
bool CMorphwizardDoc::HasModifiedSlf() const
{
	// if(	m_Wizard.m_ReadOnly ) return FALSE; 
	try {
		CSLFDocument* doc, *doc1;
		POSITION pos = m_slfDocs.GetStartPosition();
		while( !!pos ) 
		{
			m_slfDocs.GetNextAssoc(pos,doc,doc1);
			doc->GetSLFView()->UpdateData();

			if( doc->IsModified() )
				return true;
		}
	}
	catch ( ... )
	{
		ErrorMessage ("An exception in  CMorphwizardDoc::HasModifiedSlf");
	};
	return false;
}

//----------------------------------------------------------------------------
BOOL CMorphwizardDoc::IsModified()
{
	//if(	GetWizard()->m_ReadOnly ) return FALSE; 
	if( GetWizard()->is_changed() || HasModifiedSlf() ) return TRUE;
	return FALSE;
}

//----------------------------------------------------------------------------
BOOL CMorphwizardDoc::OnOpenDocument(LPCTSTR lpszPathName)
{

	try {
		if (!CDocument::OnOpenDocument(lpszPathName))
            return FALSE;
		return TRUE;
	}
	catch(CExpc& C)
	{
		ErrorMessage("Unexpected  exception : "+ C.m_strCause);
		
	}
	catch(...)
	{
		ErrorMessage("Unexpected  exception");
	}

	return FALSE;
}

BOOL CMorphwizardDoc::CanCloseFrame(CFrameWnd* pFrame)
{
/*
	CMorphwizardApp* App = (CMorphwizardApp*)AfxGetApp();
	POSITION pos = GetFirstViewPosition();
	assert(pos);
	CMorphwizardView* pView = (CMorphwizardView*)GetNextView( pos );

	if (App->m_PointerCount[pView] > 0) 
	{
		AfxMessageBox ("Close all slf-windows");
		return FALSE;
	}
*/
	return CDocument::CanCloseFrame(pFrame);
}

//----------------------------------------------------------------------------
void CMorphwizardDoc::OnCloseDocument()
{
	CSLFDocument* doc, *doc1;
	POSITION pos = m_slfDocs.GetStartPosition();
	while( !!pos ) 
	{
		m_slfDocs.GetNextAssoc(pos,doc,doc1);
		doc->m_bShouldBeClosed;
		doc->OnCloseDocument();
	}

	CDocument::OnCloseDocument();

	SetInputLanguage(morphEnglish);
}

//----------------------------------------------------------------------------
BOOL CMorphwizardDoc::SaveModified()
{
	CSLFDocument* doc, *doc1;
	CString s;
	if( HasModifiedSlf() ) 
	{
		POSITION pos = m_slfDocs.GetStartPosition();
		while( !!pos ) 
		{
			m_slfDocs.GetNextAssoc(pos,doc,doc1);
			if( doc->IsModified() )
				s += doc->GetLemma() + ",\n";
		}
	}

	bool hasModifedSlf = !s.IsEmpty();
	int rn=IDCANCEL;

	if( hasModifedSlf || IsModified() ) 
	{
		CString msg; 
		if( hasModifedSlf )
		{
			s.Delete(s.GetLength()-2,2);
			msg = GetTitle() + "\n\nSave all ";
			if( GetWizard()->is_changed() ) 
				msg += "changes\nincluding ";

			msg += "revised paradigms:\n\n" + s + "?";
		}
		else	// if( IsModified() )
		{
			msg = GetTitle() + "\n\nSave all changes?";
		}

		rn = ::AfxMessageBox(msg, MB_YESNOCANCEL);

		if( rn==IDCANCEL ) 
			return FALSE;
	}

	if( hasModifedSlf && rn==IDYES ) 
	{
		POSITION pos = m_slfDocs.GetStartPosition();
		while( !!pos ) 
		{
			m_slfDocs.GetNextAssoc(pos,doc,doc1);
			doc->DoFileSave();
		}
	}

	if( rn==IDYES ) 
		DoFileSave();

	return TRUE;
}

//----------------------------------------------------------------------------
/*
BOOL CMorphwizardDoc::SaveModified()
{
	if (!IsModified())
		return TRUE;        // ok to continue

	// get name/title of document
	CString name;
	if (m_strPathName.IsEmpty())
	{
		// get name based on caption
		name = m_strTitle;
		if (name.IsEmpty())
			VERIFY(name.LoadString(AFX_IDS_UNTITLED));
	}
	else
	{
		// get name based on file title of path name
		name = m_strPathName;
		AfxGetFileTitle(m_strPathName, name.GetBuffer(_MAX_PATH), _MAX_PATH);
		name.ReleaseBuffer();
	}

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}
*/
