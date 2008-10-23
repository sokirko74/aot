// ArticleDoc.cpp : implementation of the CArticleDoc class
//


#include "stdafx.h"
#include "font.h"
#include "ArticleDoc.h"
#include "Wordlist.h"
#include "../common/utilit.h"
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

CArticleDoc::CArticleDoc()
{

//  m_MessageKiller = new CMessageBoxKillerThread (AfxGetApp()->m_pMainWnd->m_hWnd);
//  m_MessageKiller->CreateThread();
   
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


void CArticleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		m_WasSaved = true;
        
		if (!Markout ()) 
		{  
			AfxMessageBox (GetArticleErrorStr().c_str());
			m_WasSaved = false;
		    return;
		};

		try {
			
			m_Article.SetArticleStr( GetArticleView ()->GetText() );
			m_Article.WriteToDictionary();
			if (m_pRossDoc->m_Author != "")
			{
				m_pRossDoc->GetRoss()->SetUnitEditor(m_UnitNo, (const char*)m_pRossDoc->m_Author);
			}

			GetRoss()->SetUnitCurrentTime(m_UnitNo);
		}
		catch (...){
			CString prompt = GetArticleErrorStr().c_str();
			if (prompt != "")
				AfxMessageBox (prompt);
			else
				AfxMessageBox ("Cannot write article (Structdict)");
		};
		

		((CWordList*)(m_pRossDoc->GetWordList()))->UpdateIndex();
		m_pRossDoc->SetModifiedFlag(TRUE);
		try {
			CString S  = m_pRossDoc->m_Author;
			if (S == "") S = "user";
			S += "." + CString(m_pRossDoc->GetRossHolder()->m_DictName.c_str()) + ".log";
			S = m_pRossDoc->GetRossHolder()->m_DictName.c_str()+S;
			FILE * fp = fopen(S, "ab");
			fseek(fp,SEEK_END,0); 
			fprintf (fp,"============\r\n");
			fprintf (fp,"%s",GetRoss()->GetUnitTextHeader(m_UnitNo).c_str());
		    try {
				string strPath = GetRegistryString("SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName\\ComputerName");
				fprintf (fp, "//MachineName = %s \r\n", strPath.c_str());
				fprintf (fp,"//Date = %s\r\n", CTime::GetCurrentTime().Format( "%A, %B %d, %Y " )); 
				fprintf (fp,"//Time = %s\r\n", CTime::GetCurrentTime().Format( "%H : %M" )); 
				CString UserName = getenv("USERNAME");
				if (!UserName.IsEmpty())
					fprintf (fp, "//UserName = %s \r\n", UserName);

			}
		    catch (...)
			{
			};

			fprintf (fp, "%s", (const char*) GetArticleView ()->GetText());
			fclose(fp);
		}
		catch (...){
			AfxMessageBox ("Cannot write backup");
		};
	}
	else
	{   
	}
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
	int Height = from.GetSize().Lo/10000;
	Height = -MulDiv(Height, dc.GetDeviceCaps(LOGPIXELSY), 72);
	to.lfHeight = Height;
	to.lfWidth 	= 0 ;
	to.lfEscapement 	= 0 ;
	to.lfOrientation 	= 0 ;
	to.lfWeight 		= from.GetWeight();
	to.lfItalic 		= from.GetItalic();
	to.lfUnderline 	= from.GetUnderline();
	to.lfStrikeOut 	= from.GetStrikethrough();
	to.lfCharSet		= from.GetCharset();
	to.lfOutPrecision = OUT_TT_PRECIS ;
	to.lfClipPrecision= CLIP_DEFAULT_PRECIS ;
	to.lfQuality 		= PROOF_QUALITY ;
	to.lfPitchAndFamily = VARIABLE_PITCH | TMPF_TRUETYPE | FF_MODERN ;
	strcpy(to.lfFaceName, from.GetName()) ;  
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
	to.SetName("Arial"/*from.lfFaceName*/);   
}



BOOL CArticleDoc::OpenArticle (WORD  UnitNo, CRossDoc* pRossDoc)
{
	m_UnitNo = UnitNo;
	m_pRossDoc = pRossDoc;

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

	SetPathName(GetRoss()->GetEntryStr(m_UnitNo).c_str());

	// open an existing document
	GetDocTemplate()->InitialUpdateFrame(pFrame, this, TRUE);

    

	// Инициализация  CArticleView
    CArticleView* View = GetArticleView ();
	
	m_Article.m_pRoss = GetRoss();
	m_Article.ReadFromDictionary(UnitNo, true, false);

	string Article  = m_Article.GetArticleStr();
	if (Article.length() > 0 )
		View->SetText(Article.c_str());

    if(!m_pRossDoc->m_bArticleInitTextMode)
	{
	   Markout();	
	   if (!m_Article.m_Fields.empty())
	     View->SetFocusToField (0);
	}

	
	
		

	return TRUE;
};


string CArticleDoc::GetArticleErrorStr() const
{
	string  s =  m_Article.m_LastError;
	if ( s.empty() ) return s;
	int ErrorLine = m_Article.m_ErrorLine;
	if (ErrorLine != -1)
			s += Format(" (line %i)", ErrorLine);
	return s;
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
        prompt.Format("Статья \"%s\" содержит ошибки (%s) . Выйти без сохранения?", name, GetArticleErrorStr().c_str());
		
		switch (AfxMessageBox (prompt, MB_OKCANCEL))
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


bool CArticleDoc :: AddCortegeToVector (vector<TCortege10>& L, CRossDevTextField& F)
{

	 vector<CString>  Lines;
	 size_t Count = m_Article.GetCortegesSize();
     for (size_t i=0;  i < Count; i++)
	 { 
	   const TCortege10& C = m_Article.GetCortege(i);
	   if  (   (C.m_FieldNo == F.FieldNo)
		    && (C.m_LeafId  == F.LeafId)
			&& (C.m_BracketLeafId  == F.BracketLeafId)
			&& (C.m_DomItemNos[0] != -1) )
       L.push_back(C);
	 };

	 return true;
};


bool CArticleDoc :: Markout ()
{

	try    
	{
		CString SaveArticleStr = GetArticleView()->GetText();
		if (m_Article.SetArticleStr(GetArticleView()->GetText()))
		{
			string  A = m_Article.GetArticleStr();
			GetArticleView()->SetText(A.c_str());

			GetArticleView()->m_FldScroll.SetScrollRange (0, (m_Article.m_Fields.empty() ? 0 : m_Article.m_Fields.size() - 1));
			//GetArticleView()->m_RichEdit.SetReadOnly(TRUE);
			GetArticleView()->m_FldScroll.ShowWindow (SW_SHOW);
			GetArticleView()->m_RichEdit.SetFocus();
		}
		else
		{
			ErrorMessage (m_Article.m_LastError);
			return false;
		}
	}
	catch (...)
	{
		return false;
	};

	return true;
};


BOOL CArticleDoc::IsModified()
{
  if (!Markout ()) return TRUE;
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
	CString S (GetRoss()->GetEntryStr(m_UnitNo).c_str());
	char s[10];
	sprintf (s, "%i", GetRoss()->GetUnitMeanNum(m_UnitNo));
	S += CString (s); 
	S = CString(m_pRossDoc->GetRossHolder()->m_DictName.c_str())+":   "+S; 
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
    if ( !m_WasSaved ) return FALSE;

	SetModifiedFlag(FALSE);     // back to unmodified

	return TRUE;        // success
}
