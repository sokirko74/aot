// ArticleDoc.cpp : implementation of the CArticleDoc class
//


#include "StdAfx.h"
#include "font.h"
#include "ArticleDoc.h"
#include "WordList.h"
#include "morph_dict/common/utilit.h"
#include <sys/timeb.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CArticleDoc

IMPLEMENT_DYNCREATE(CArticleDoc, CDocument)

BEGIN_MESSAGE_MAP(CArticleDoc, CDocument)
	//{{AFX_MSG_MAP(CArticleDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CArticleDoc construction/destruction

CArticleDoc::CArticleDoc(): m_Article(nullptr)
{


}

CArticleDoc::~CArticleDoc()
{
}

BOOL CArticleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CArticleDoc diagnostics

#ifdef _DEBUG
void CArticleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CArticleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArticleDoc commands



void _AfxFontConvertOLE2LOG(CDC& dc, COleFont& from, LOGFONT& to)
{
	int Height = from.GetSize().Lo / 10000;
	Height = -MulDiv(Height, dc.GetDeviceCaps(LOGPIXELSY), 72);
	to.lfHeight = Height;
	to.lfWidth = 0;
	to.lfEscapement = 0;
	to.lfOrientation = 0;
	to.lfWeight = from.GetWeight();
	to.lfItalic = from.GetItalic();
	to.lfUnderline = from.GetUnderline();
	to.lfStrikeOut = from.GetStrikethrough();
	to.lfCharSet = from.GetCharset();
	to.lfOutPrecision = OUT_TT_PRECIS;
	to.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	to.lfQuality = PROOF_QUALITY;
	to.lfPitchAndFamily = VARIABLE_PITCH | TMPF_TRUETYPE | FF_MODERN;
	lstrcpy(to.lfFaceName, from.GetName());
}

void _AfxFontConvertLOG2OLE(CDC& dc, LOGFONT& from, COleFont& to)
{
	CY Height;
	//Height.Hi = 0;
//	Height.Lo = -MulDiv(from.lfHeight, 20, dc.GetDeviceCaps(LOGPIXELSY)) * 10000;
	to.SetSize(Height);
	to.SetWeight(from.lfWeight);
	to.SetItalic(TRUE/*from.lfItalic*/);
	to.SetUnderline(from.lfUnderline);
	to.SetStrikethrough(from.lfStrikeOut);
	to.SetCharset(from.lfCharSet);
	to.SetName(_T("Arial"/*from.lfFaceName*/));
}



BOOL CArticleDoc::OpenArticle(uint16_t  UnitNo, CRossDoc* pRossDoc)
{
	m_UnitNo = UnitNo;
	m_pRossDoc = pRossDoc;
	m_Article = CTempArticle(pRossDoc->GetRoss());

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

	SetPathName(_U16(GetRoss()->GetEntryStr(m_UnitNo)));

	// open an existing document
	GetDocTemplate()->InitialUpdateFrame(pFrame, this, TRUE);



	// Инициализация  CArticleView
	CArticleView* View = GetArticleView();

	m_Article.ReadFromDictionary(UnitNo, true, false);

	std::string Article = m_Article.GetArticleStrUtf8();
	if (Article.length() > 0)
		View->SetText(Article.c_str());

	if (!m_pRossDoc->m_bArticleInitTextMode)
	{
		Markout();
		if (!m_Article.m_Fields.empty())
			View->SetFocusToField(0);
	}





	return TRUE;
};


BOOL CArticleDoc::SaveModified()
{
	CString name;
	name = GetTitle();
	CString prompt;

	try
	{
		if (!IsModified())
			return TRUE;        // ok to continue
	}
	catch (...)
	{
		prompt.Format(_T("Unit \"%s\" contains errors. Exit without save?"), name);

		switch (AfxMessageBox(prompt, MB_OKCANCEL))
		{
		case IDCANCEL:
			return FALSE;
		case IDOK:
			return TRUE;
			break;
		default:
			ASSERT(FALSE);
			break;
		};
	};

	// get name based on file title of path name
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

	}
	return TRUE;    // keep going

};




bool CArticleDoc::Markout()
{
	try
	{
		m_Article.ReadFromUtf8String(_U8(GetArticleView()->GetText()).c_str());
		std::string  A = m_Article.GetArticleStrUtf8();
		GetArticleView()->SetText(_U16(A));

		GetArticleView()->m_FldScroll.SetScrollRange(0, (m_Article.m_Fields.empty() ? 0 : m_Article.m_Fields.size() - 1));
		GetArticleView()->m_FldScroll.ShowWindow(SW_SHOW);
		GetArticleView()->m_RichEdit.SetFocus();
	}
	catch (std::exception& a)
	{
		AfxMessageBox(_U16(a.what()));
		return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
};


BOOL CArticleDoc::IsModified()
{
	if (!Markout()) return TRUE;
	return m_Article.IsModified() ? TRUE : FALSE;
};


BOOL CArticleDoc::DoFileSave()
{
	try
	{
		return DoSave(m_strPathName);
	}
	catch (...)
	{
		return FALSE;
	};
};

void CArticleDoc::SetTitle(LPCTSTR lpszTitle)
{
	CString S(GetRoss()->GetEntryStr(m_UnitNo).c_str());
	char s[10];
	sprintf(s, "%i", GetRoss()->GetUnitMeanNum(m_UnitNo));
	S += CString(s);
	S = CString(m_pRossDoc->GetRossHolder()->GetDictName().c_str()) + ":   " + S;
	CDocument::SetTitle(S);
}



BOOL CArticleDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	CFile p;
	CArchive saveArchive(&p, CArchive::store | CArchive::bNoFlushOnDelete);
	TRY
	{
		CWaitCursor wait;
		Serialize(saveArchive);     // save me
		saveArchive.Close();
	}
		CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
		if (!m_WasSaved) return FALSE;

	SetModifiedFlag(FALSE);     // back to unmodified

	return TRUE;        // success
}
