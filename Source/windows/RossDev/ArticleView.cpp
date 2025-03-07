
#include "StdAfx.h"
#include "ArticleView.h"
#include "ArticleDoc.h"
#include "morph_dict/common/utilit.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArticleView

IMPLEMENT_DYNCREATE(CArticleView, CSizeFormView)

CArticleView::CArticleView()
	: CSizeFormView(CArticleView::IDD)
{
	m_StandardLogFont.lfFaceName[0] = 0;
}

CArticleView::~CArticleView()
{
}

inline CDictionary* CArticleView::GetRoss ()
 {return ((CArticleDoc*)GetDocument())->GetRoss();};

inline CRossDoc* CArticleView::GetRossDoc ()
 {return ((CArticleDoc*)GetDocument())->m_pRossDoc;};


void CArticleView::DoDataExchange(CDataExchange* pDX)
{
	CSizeFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArticleView)
	DDX_Control(pDX, IDC_FLDSCROLL, m_FldScroll);
	DDX_Control(pDX, IDC_RICHEDIT21, m_RichEdit);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CArticleView, CSizeFormView)
	//{{AFX_MSG_MAP(CArticleView)
	ON_BN_CLICKED(IDC_ARTICLEVIEW_MARKUP, OnArticleviewMarkup)
	ON_BN_CLICKED(IDC_ARTICLEVIEW_TEXTBTN, OnArticleviewTextbtn)
	ON_BN_CLICKED(IDC_ARTICLE_VIEW_DEL_FIELD, OnArticleViewDelField)
	ON_EN_CHANGE(IDC_RICHEDIT21, OnChangeRichedit1)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)


	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArticleView diagnostics

#ifdef _DEBUG
void CArticleView::AssertValid() const
{
	CSizeFormView::AssertValid();
}

void CArticleView::Dump(CDumpContext& dc) const
{
	CSizeFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArticleView message handlers

void CArticleView::OnArticleformClose() 
{
   GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 1);
}


void CArticleView::OnArticleviewMarkup() 
{
	try
	{
		((CArticleDoc*)GetDocument())->Markout();	

		if (!((CArticleDoc*)GetDocument())->m_Article.m_Fields.empty())
			SetFocusToField (0);

		m_FldScroll.SetScrollPos(0,TRUE);
		parse();
	}
	catch (std::exception a)
	{
		ErrorMessage("Rossdev", a.what());
	}
	catch (...)
	{
		ErrorMessage( "Rossdev", "Unknown error!");
	};
	
}

inline int GetVisibleLinesCount(const CRichEditCtrl& C)
{
	CRect R;
	C.GetWindowRect(&R);
	CHARFORMAT cf;
	C.GetDefaultCharFormat(cf);
	HDC lngDC = ::GetDC(HWND_DESKTOP);
	double TwipsPerPixelX = 1440 / GetDeviceCaps(lngDC, LOGPIXELSX);
	int LineInPixel = (cf.yHeight + cf.yOffset) / TwipsPerPixelX;
	return R.Height() / LineInPixel;
};



void CArticleView::SetFocusToField (int nPos)
{
	m_RichEdit.SetFocus();
	const CTempArticle& A = ((CArticleDoc*)GetDocument())->m_Article;
	if (!A.m_Fields.empty())
		if ( nPos < A.m_Fields.size() )
		{

			const CTextField& T = A.m_Fields[nPos];
			int VisibleLinesCount = GetVisibleLinesCount(m_RichEdit);
			if ( m_RichEdit.GetLineCount() < VisibleLinesCount )
				m_RichEdit.LineScroll(0);
			else
				m_RichEdit.LineScroll(T.StartLine - (VisibleLinesCount - (T.EndLine - T.StartLine + 1)) + 1);

			long Begin, End;
			Begin = m_RichEdit.LineIndex(T.StartLine);
			End = m_RichEdit.LineIndex(T.EndLine + 1);
			m_RichEdit.SetSel(Begin, End);
			m_RichEdit.UpdateData();
		};
};

void CArticleView::OnArticleviewTextbtn() 
{
	 m_FldScroll.ShowWindow (SW_HIDE);
     m_RichEdit.SetFocus();
}






void CArticleView::SelectLine ( long LineNo )
{
	long nStartChar = m_RichEdit.LineIndex(LineNo);
	long nEndChar = m_RichEdit.LineIndex(LineNo+1); 
	m_RichEdit.SetSel(nStartChar, nEndChar);
};

void CArticleView::DeleteLine(size_t i)
{
	SelectLine(i);

	m_RichEdit.ReplaceSel(_T(""));
};


void CArticleView::DelField(size_t nPos) 
{
	CTempArticle& A = ((CArticleDoc*)GetDocument())->m_Article;

	//  deleting lines in the editor
	if (A.m_Fields.size() == 1)
		m_RichEdit.Clear();
	else
		for (int i=A.m_Fields[nPos].StartLine; i <=  A.m_Fields[nPos].EndLine; i++)
			DeleteLine (A.m_Fields[nPos].StartLine); 


	//  updating windows
	((CArticleDoc*)GetDocument())->Markout();

	//  change scroll bar
	if (!A.m_Fields.empty())
	{
		m_FldScroll.SetScrollRange (0, A.m_Fields.size() - 1);
		if (nPos> m_FldScroll.GetScrollPos()) nPos --;
		m_FldScroll.SetScrollPos(nPos);
		SetFocusToField (nPos);
	}
	else
		m_FldScroll.SetScrollRange (0, 0);
};

void CArticleView::OnArticleViewDelField() 
{
	const CTempArticle& A = ((CArticleDoc*)GetDocument())->m_Article;
    if (A.m_Fields.empty()) return;
	DelField(m_FldScroll.GetScrollPos());
}


void CArticleView::InsertLine (int LineNo, const CString& S)
{
	CString Line  = S + "\r\n";
	long nStartChar = m_RichEdit.LineIndex(LineNo);
	m_RichEdit.SetSel(nStartChar, nStartChar);
	m_RichEdit.ReplaceSel(Line);
	parse();
};

void CArticleView::WriteToEdit (std::vector<TCortege>& L, size_t nPos)
{
	const CTempArticle& A = ((CArticleDoc*)GetDocument())->m_Article;
	if (A.m_Fields.size() == 1)
		m_RichEdit.Clear();
	else
		for (int i=A.m_Fields[nPos].StartLine; i <=  A.m_Fields[nPos].EndLine; i++)
			DeleteLine (A.m_Fields[nPos].StartLine); 
		size_t LevelId = 0;
	CString S;
	// сначала записываем первую строку, т.к. она сильно отличается от всех
	if (L.size() > 0)
	{
		std::string Q = GetRoss()->WriteToString(L[0]);
		LevelId = L[0].m_LevelId;
		if ( LevelId == 1 )  
		{
			std::string t = "1 ";
			t +=  Q;
			Q = t.c_str();
		};

		CString FieldStr =  ConstructFldName(GetRoss(), A.m_Fields[nPos].FieldNo, A.m_Fields[nPos].LeafId, A.m_Fields[nPos].BracketLeafId);
		S.Format (_U16(_FieldFormat), FieldStr, (const TCHAR*)_U16(Q));
	}
	else
		S.Format (_U16(_FieldFormat), ConstructFldName(GetRoss(), A.m_Fields[nPos].FieldNo, A.m_Fields[nPos].LeafId, A.m_Fields[nPos].BracketLeafId)," ");


	InsertLine (A.m_Fields[nPos].StartLine, S);

	for (size_t i=1; i<L.size(); i++)
	{
		std::string Q = GetRoss()->WriteToString(L[i]);
		// cмотри выше Положение о выравнивании
		if (L[i].m_LevelId > LevelId)
		{
			LevelId = L[i].m_LevelId;
			S.Format (_T("%i %s"), LevelId, Q.c_str());

		}
		else
			if (LevelId > 0)
			{
				S = CString("  ")+Q.c_str();
			}
			else
				S = Q.c_str();

		CString R;
		R.Format (_T("%10s%s")," ",S);

		InsertLine (i+A.m_Fields[nPos].StartLine,R);
	}

	// StartLine не изменился, а EndLine нужно вычислить заново
	((CArticleDoc*)GetDocument())->Markout();
};


bool IsRossEntry (const char* ItemStr)
{
	CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
    POSITION pos = pRossDocTemplate->GetFirstDocPosition();
	while( pos )
	{
		CRossDoc* pDoc = (CRossDoc*)pRossDocTemplate->GetNextDoc(pos);	
		if (pDoc->GetRoss()->LocateUnit(ItemStr, 1) != ErrUnitNo) return true;
	};

	return false;
};



BOOL CArticleView::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CSizeFormView::PreCreateWindow(cs);
}

bool IsKeyWordArticle (const CString& word, COLORREF& C, uint32_t Data)
{
	CArticleView* V = (CArticleView*)Data;
	CRossDoc* RossDoc = V->GetRossDoc();
	auto word_u8 = _U8(word);
	auto it = RossDoc->m_BasicDomItems.find(word_u8);

    if (    (it == RossDoc->m_BasicDomItems.end())
		|| (  RossDoc->GetRoss()->m_Domens[it->second].GetDomStr() == std::string("D_1"))) // если это не элемент метаязыка
	{
		if (IsRossEntry(word_u8.c_str()))
		{
			C = RGB(0,0,255);
			return true;
		};
	}
	else
	{
		
		BYTE no = RossDoc->m_DomParamsPtr[it->second];
		if (no == ErrUChar) return 0;
		C = RossDoc->m_DomainParams[no].Color;
		return true;
	};

	auto field_it = RossDoc->m_Fields.find(word_u8);
	if 	(field_it != RossDoc->m_Fields.end())
	{
		C = RGB(90,90,90);
		return true;
	};
	return false;
};


void CArticleView::parse()
{
	long nBegin, nEnd;
	m_RichEdit.GetSel(nBegin, nEnd);

	//turn off response to onchange events
	long mask = m_RichEdit.GetEventMask();
	m_RichEdit.SetEventMask(mask ^= ENM_CHANGE );

	//set redraw to false to reduce flicker, and to speed things up
	m_RichEdit.SetRedraw(FALSE);

	//call the colorizer
	m_Colorizer.Colorize(&m_RichEdit, 0,-1);

	//do some cleanup
	m_RichEdit.SetSel(0,0);
	m_RichEdit.SetRedraw(TRUE);
	m_RichEdit.RedrawWindow();

	m_RichEdit.SetEventMask(mask |= ENM_CHANGE );
	m_RichEdit.SetSel(nBegin, nEnd);
}

void CArticleView::parse2()
{
	//get the current line of text from the control
	int len = m_RichEdit.LineLength();
	int start = m_RichEdit.LineIndex();

	//call the colorizer
	m_Colorizer.Colorize(&m_RichEdit, start,start + len);
}

void CArticleView::OnChangeRichedit1() 
{
	CHARRANGE cr;
	m_RichEdit.GetSel(cr);
	parse2();
	m_RichEdit.SetSel(cr);
}


void CArticleView::OnInitialUpdate() 
{
	CSizeFormView::OnInitialUpdate();
	m_FldScroll.ShowWindow (SW_HIDE);
    m_RichEdit.SetFocus();

	m_Colorizer.InitializeParser(IsKeyWordArticle, (uint32_t)this);

	long mask = m_RichEdit.GetEventMask();
	m_RichEdit.SetEventMask(mask |= ENM_CHANGE );

}

void CArticleView::SetText(CString S)
{
	S.Replace(_T("\n"), _T("\r\n"));
	m_RichEdit.SetWindowText(S);
	CString debug = GetText();
	parse();
	debug = GetText();
};

CString CArticleView::GetText() const
{
	CString A;
	m_RichEdit.GetWindowText(A);
	return  A;
};


IMPLEMENT_DYNCREATE(CMyEditCtrl, CRichEditCtrl)
BEGIN_MESSAGE_MAP(CMyEditCtrl, CRichEditCtrl)
		ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

inline bool CanBeEntrySymbol(int c)
{
	return		is_english_alpha(c)
			||  c > 127
			||  c == '_'
			||	c == '-';
};

void CMyEditCtrl::OnLButtonDblClk( UINT nFlags,  CPoint point )
{
	CArticleView* V = (CArticleView*)GetParent();
	int CharNo = CharFromPos(point);
	int LineNo = LineFromChar(CharNo);
	CString SourceText;
	GetWindowText(SourceText);
	SourceText.Replace(_T("\r\n"), _T("\n"));
	int start = CharNo;
	for (; start >=0; start--)
		if (!CanBeEntrySymbol(SourceText.GetAt(start) ) )
			break;
	start++;
	if (start > CharNo) return;
	int end = CharNo;
	for (; end < SourceText.GetLength(); end++)
		if ( !CanBeEntrySymbol(SourceText.GetAt(end) ) )
			break;

	CString Word =  SourceText.Mid(start, end-start);

	BYTE MeanNum = 1;

	for (int MeanNumStart = end; MeanNumStart < SourceText.GetLength(); MeanNumStart++)
		if ( (unsigned char)SourceText.GetAt(MeanNumStart) == '\n')
			break;
		else
			if ( isdigit((unsigned char)SourceText.GetAt(MeanNumStart)) )
			{
				MeanNum = SourceText.GetAt(MeanNumStart) - '0';
				break;
			};




	CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
	POSITION pos = pRossDocTemplate->GetFirstDocPosition();
	while( pos )
	{
		CRossDoc* pDoc = (CRossDoc*)pRossDocTemplate->GetNextDoc(pos);	
  		uint16_t UnitNo = pDoc->GetRoss()->LocateUnit(_U8(Word).c_str(), MeanNum);
		if (UnitNo != ErrUnitNo) 
		{
			GlobalOpenArticle (pDoc, UnitNo);
			return;
		};
	};

};


void CArticleView::OnEditCopy() 
{
	m_RichEdit.Copy();
}

void CArticleView::OnEditPaste() 
{
	m_RichEdit.Paste();

}


void CArticleView::OnEditCut() 
{
	m_RichEdit.Cut();

}

void CArticleView::OnEditUndo() 
{
	m_RichEdit.Undo();

}