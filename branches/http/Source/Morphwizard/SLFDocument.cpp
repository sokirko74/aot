// SLFDocument.cpp : implementation file
//

#include "stdafx.h"
#include "Morphwizard.h"
#include "SLFDocument.h"
#include "SLFView.h"
#include "MorphwizardView.h"
#include "MorphwizardDoc.h"


IMPLEMENT_DYNCREATE(CSLFDocument, CDocument)
BEGIN_MESSAGE_MAP(CSLFDocument, CDocument)
END_MESSAGE_MAP()

CSLFDocument::CSLFDocument()  
{
	m_GotoLine = -1;
	m_bShouldBeClosed = false;
}

BOOL CSLFDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CSLFDocument::~CSLFDocument()
{
}
MorphoWizard*	CSLFDocument::GetWizard()
{
	return m_pParent->GetWizard();
};

const MorphoWizard*	CSLFDocument::GetWizard() const
{
	return m_pParent->GetWizard();
};
//----------------------------------------------------------------------------
CString CSLFDocument::GetSavedLemma() 
{
	StringTokenizer t (m_SaveParadigmText," ");
	
	if (t())
	{
		std::string Lemma = t.val();
		RmlMakeUpper(Lemma, GetWizard()->m_Language);
		return Lemma.c_str();
	}
	return "";
}

//----------------------------------------------------------------------------
CString CSLFDocument::GetLemma() const
{
	StringTokenizer t (m_ParadigmText," ");

	if (t())
	{
		string s = t.val();
		string Lemma;
		for (size_t i=0; i<s.length(); i++)
			if ((BYTE)s[i] != '\'')
				Lemma += s[i];

		RmlMakeUpper(Lemma, GetWizard()->m_Language);

		return Lemma.c_str();
	}
	return "";
}

string CSLFDocument::GetBase()  const 
{
	string Lemma = (const char*)GetLemma();
	if (m_Paradigm.m_FlexiaModelNo != UnknownParadigmNo)
	{
		const CFlexiaModel &old_par = GetWizard()->m_FlexiaModels[m_Paradigm.m_FlexiaModelNo];
		std::string flexia = old_par.get_first_flex();
		return Lemma.substr(0, Lemma.length() - flexia.length());
	}
	else
		return "";
}
	



// CSLFDocument diagnostics

#ifdef _DEBUG
void CSLFDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CSLFDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


BOOL CSLFDocument::IsModified()
{
	if (m_bShouldBeClosed) return FALSE;

	// if the text of the saved paradigm  and the edited paradigm differ, then "it was  modified"
	if	(		m_SaveParadigmText != m_ParadigmText 
			||	m_SaveCommonGrammems != m_CommonGrammems
			||	m_SavePrefixes != m_Prefixes
		)
		return TRUE;

	
	//  if a new lemma was edited, then there is no sense to search  it in the dictionary
	if (m_SaveParadigmText.IsEmpty()) return TRUE;


	// Since we have a MDI environment,  the saved lemma can be deleted 
	// while an editor for this  lemma is still open. In this case we should  say to the user
	// that this paradigm "was modified" and propose to insert it into the dictionary.
	std::vector<lemma_iterator_t> res;
	GetWizard()->find_lemm((const char*)GetSavedLemma(), false, res);
	for (size_t i=0; i<res.size(); i++)
		if (res[i]->second == m_SaveParadigm)
			return FALSE;

	return TRUE;
	
}

//----------------------------------------------------------------------------
BOOL CSLFDocument::SaveModified()
{
	if (!IsModified())
			return TRUE;        // ok to continue

	m_ParadigmText.TrimLeft();
	if (!m_ParadigmText.IsEmpty())
	try
    {
		std::string slf  = (const char*)m_ParadigmText;
		std::string common_grammems  = (const char*)m_CommonGrammems;
		
		std::string lemm;

		//  try to parse a paradigm (waiting for an exception otherwise)
		{
			CFlexiaModel Dummy1;
			CAccentModel AccentModel;
			BYTE Dummy;
			GetWizard()->slf_to_mrd (slf, lemm, Dummy1, AccentModel, Dummy, m_GotoLine);
			if (!GetWizard()->check_common_grammems(common_grammems))
				throw CExpc("Bad type grammems");

			if (!GetWizard()->check_prefixes((const char*)m_Prefixes))
				throw CExpc("Bad prefixes");

		}

		
		// deleting old lemma
		if (!m_SaveParadigmText.IsEmpty())
		{
			std::vector<lemma_iterator_t> res;
			GetWizard()->find_lemm((const char*)GetSavedLemma(), false, res);
			for (size_t i=0; i<res.size(); i++)
				if (res[i]->second == m_SaveParadigm)
				{
					GetWizard()->remove_lemm(res[i]);
					break;
				};
		};


		// writing
		m_Paradigm = GetWizard()->add_lemma((const char*)m_ParadigmText, (const char*)m_CommonGrammems, (const char*)m_Prefixes, m_GotoLine);

		update_saved_paradigm(); //  [4/Dec/2003]
	}
	catch(CExpc& e)
	{
		switch (AfxMessageBox (Format("%s at line %i\nExit without save?", e.m_strCause.c_str(), m_GotoLine  ).c_str(), MB_OKCANCEL) )
		{
		   case IDCANCEL:return FALSE;  	  	   
		   case IDOK: return TRUE;
		   default:  ASSERT(FALSE); break;
		}
	}

	return TRUE;    // keep going
}

// CSLFDocument serialization

CSLFView* CSLFDocument::GetSLFView()
{
    POSITION pos = GetFirstViewPosition();
	assert(pos);
	CView* view = GetNextView(pos);
	if( view->IsKindOf(RUNTIME_CLASS(CSLFView)) ) 
		return (CSLFView*)view;

	return NULL;
}

//----------------------------------------------------------------------------
// Nick Ketsaris [4/Dec/2003]
//----------------------------------------------------------------------------
void CSLFDocument::update_saved_paradigm()
{
	m_ParadigmText.TrimLeft();
	m_SaveParadigmText = m_ParadigmText;
	m_SaveParadigm = m_Paradigm;
	m_SaveCommonGrammems = m_CommonGrammems;	
	m_SavePrefixes = m_Prefixes;	
	if (m_Paradigm.m_FlexiaModelNo != UnknownParadigmNo)
		m_ParadigmComments = GetWizard()->m_FlexiaModels[m_Paradigm.m_FlexiaModelNo].m_Comments.c_str();	

	if (m_ParadigmText.GetLength() > 0)
		SetPathName(GetSavedLemma());
}

//----------------------------------------------------------------------------
BOOL CSLFDocument::OpenParadigm( bool runAccentWizard )
{
	BOOL bAutoDelete = m_bAutoDelete;
	m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
	CFrameWnd* pFrame = GetDocTemplate()->CreateNewFrame(this, NULL);
	m_bAutoDelete = bAutoDelete;

	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete this;       // explicit delete on error
		return FALSE;
	};

	ASSERT_VALID(pFrame);

	SetPathName("new paradigm");

	update_saved_paradigm(); // Nick [4/Dec/2003]

	// open an existing document
	GetDocTemplate()->InitialUpdateFrame(pFrame, this, TRUE);

	CSLFView* view = GetSLFView();
	AddView( view->m_pRichView );
	view->m_bRunAccentWizard = runAccentWizard;


	// Nick 30.11.2003
	if( !pFrame->IsZoomed() ) 
		pFrame->SetWindowPos(0, 50, 50, 800, 600, 0 );
	
	// Nick 30.11.2003
	
	m_pParent->GetDocument()->m_slfDocs[this] = this;

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSLFDocument::OpenDiffDialog ()
{
	BOOL bAutoDelete = m_bAutoDelete;
	m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
	CFrameWnd* pFrame = GetDocTemplate()->CreateNewFrame(this, NULL);
	m_bAutoDelete = bAutoDelete;

	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete this;       // explicit delete on error
		return FALSE;
	}
	ASSERT_VALID(pFrame);
	SetPathName("Diff dialog");
	GetDocTemplate()->InitialUpdateFrame(pFrame, this, TRUE);
	
	if( !pFrame->IsZoomed() ) 
		pFrame->SetWindowPos(0, 50, 50, 600, 400, 0 );

	m_pParent->GetDocument()->m_slfDocs[this] = this;

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSLFDocument::DoFileSave()
{
	return SaveModified();
}

//----------------------------------------------------------------------------
BOOL CSLFDocument::OnSaveDocument(LPCTSTR lpszPathName) 
{
	return SaveModified();
}

//----------------------------------------------------------------------------
// CSLFDocument commands
void CSLFDocument::OnCloseDocument( )
{
//	CMorphwizardApp* App = (CMorphwizardApp*)AfxGetApp();
//	App->m_PointerCount[m_pParent]--;
	
	m_pParent->GetDocument()->m_slfDocs.RemoveKey(this);

	CDocument::OnCloseDocument();
}

//----------------------------------------------------------------------------
BOOL CSLFDocument::CanCloseFrame(CFrameWnd* pFrame)
{
	CSLFView* view = GetSLFView();
	if( !!view ) 
	{
		view->UpdateData();
		if( IsModified() )
		{
			if (AfxMessageBox ("Exit without save?", MB_OKCANCEL) == IDCANCEL)
				return FALSE;        
		}
		return TRUE;
	}

	return CDocument::CanCloseFrame(pFrame);
}
