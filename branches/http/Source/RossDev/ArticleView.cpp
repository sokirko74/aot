// ArticleView.cpp : implementation file
//

#include "stdafx.h"
#include "ArticleView.h"
#include "ArticleDoc.h"
#include "FieldList.h"
#include "ValueDlg.h"
#include "SetEditor.h"
#include "FormulaEditor.h"
#include "../common/utilit.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArticleView

IMPLEMENT_DYNCREATE(CArticleView, CSizeFormView)

CArticleView::CArticleView( )
	: CSizeFormView(CArticleView::IDD)
{
	//{{AFX_DATA_INIT(CArticleView)
	//}}AFX_DATA_INIT
	//m_pRossDoc = ((CRossDevApp*)AfxGetApp())->GetRossDoc();
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
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_ARTICLE_VIEW_DEL_FIELD, OnArticleViewDelField)
	ON_BN_CLICKED(IDC_ARTICLE_FORM_VALUE, OnArticleFormValue)
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
	// TODO: Add your control notification handler code here
   GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 1);
}





void CArticleView::OnArticleviewMarkup() 
{

	//long a, b;
	//m_RichEdit.GetSel(a,b);
	//AfxMessageBox (Format("GetSel() = (%i, %i)", a,b).c_str());

	try
	{
		((CArticleDoc*)GetDocument())->Markout();	

		if (!((CArticleDoc*)GetDocument())->m_Article.m_Fields.empty())
			SetFocusToField (0);

		m_FldScroll.SetScrollPos(0,TRUE);
		parse();
	}
	catch (...)
	{
		ErrorMessage( "Rossdev", ((CArticleDoc*)GetDocument())->GetArticleErrorStr());
	};
	
}




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
	 //m_RichEdit.SetReadOnly(FALSE);
     m_RichEdit.SetFocus();
}






void CArticleView::OnAddButton() 
{
    SetFocusToField (m_FldScroll.GetScrollLimit());
	AddOrInsertRossField (m_FldScroll.GetScrollLimit(),fnAdd);
};


void CArticleView::AddOrInsertRossField (int nPos, ActionType fnAction) 
{
	// TODO: Add your control notification handler code here
	vector <CRossDevTextField> ResultList;
	const CTempArticle& A = ((CArticleDoc*)GetDocument())->m_Article;
		

	CFieldList dlg (GetRossDoc(), ResultList );
	if (dlg.DoModal() != IDOK) return;
	
	for (int i=0; i < ResultList.size(); i++)
	{
		 CRossDevTextField F = ResultList[i];
		 CString S;
		 S.Format ("%-8s= ", ConstructFldName (GetRoss(), F.FieldNo, F.LeafId, F.BracketLeafId));
		 int CharIndex;
		 if (fnAction == fnAdd)
		{	
				nPos = A.m_Fields.size();
				CharIndex = m_RichEdit.GetTextLength();
			}
		 else
		 {
			 CharIndex = m_RichEdit.LineIndex(nPos);
		 };
		 m_RichEdit.SetSel(CharIndex, CharIndex);
		 m_RichEdit.ReplaceSel(S);

 		 ((CArticleDoc*)GetDocument())->Markout();
		 SetFocusToField (nPos);
		 m_FldScroll.SetScrollRange (0, m_FldScroll.GetScrollLimit() + 1);
		 m_FldScroll.SetScrollPos(nPos);
	};

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

	m_RichEdit.ReplaceSel("");
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


	//  updating COM
	((CArticleDoc*)GetDocument())->Markout();
	A.BuildCortegeList();

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

void CArticleView::EditOneValue(CRossDevTextField& F)
{
	vector<TCortege> L; //старое значение
	TCortege NewValue; //новое  значение

	CArticleDoc* D =  (CArticleDoc*)GetDocument();
	try
	{
		D->AddCortegeToVector ( L, F);
	}
	catch (...)
	{  
		L.clear();
		

  	};
	if (L.size() == 0)
	{
		TCortege C;
		C.m_FieldNo = F.FieldNo;
		C.m_LeafId = F.LeafId;
		C.m_BracketLeafId = F.BracketLeafId;
		C.m_SignatNo = 0;
		C.m_LevelId = 0;
		L.push_back(C);
	};

	CValueDlg Dlg (GetRossDoc(), L[0], NewValue);
	try 
	{
	  if (Dlg.DoModal() != IDOK) return;
	}
	catch (...)
	{
		return;
	};

	string Q = WriteToString(GetRoss(), GetRoss()->Fields[NewValue.m_FieldNo].m_Signats[NewValue.GetSignatNo()].sFrmt, NewValue);
	CString S;
	S.Format ( NewValue.GetFieldFormat(), ConstructFldName(GetRoss(), F.FieldNo, F.LeafId, F.BracketLeafId), Q.c_str());
	m_RichEdit.ReplaceSel(S);
};

void CArticleView::InsertLine (int LineNo, const CString& S)
{
	CString Line  = S + "\r\n";
	long nStartChar = m_RichEdit.LineIndex(LineNo);
	m_RichEdit.SetSel(nStartChar, nStartChar);
	m_RichEdit.ReplaceSel(Line);
	parse();
};

void CArticleView::WriteToEdit (vector<TCortege10>& L, size_t nPos)
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
		string Q = WriteToString(GetRoss(), GetRoss()->Fields[L[0].m_FieldNo].m_Signats[L[0].GetSignatNo()].sFrmt, L[0]);
		LevelId = L[0].m_LevelId;
		if ( LevelId == 1 )  
		{
			string t = "1 ";
			t +=  Q;
			Q = t.c_str();
		};

		CString FieldStr =  ConstructFldName(GetRoss(), A.m_Fields[nPos].FieldNo, A.m_Fields[nPos].LeafId, A.m_Fields[nPos].BracketLeafId);
		S.Format (_FieldFormat,FieldStr ,Q.c_str());
	}
	else
		S.Format (_FieldFormat, ConstructFldName(GetRoss(), A.m_Fields[nPos].FieldNo, A.m_Fields[nPos].LeafId, A.m_Fields[nPos].BracketLeafId)," ");


	InsertLine (A.m_Fields[nPos].StartLine, S);

	for (size_t i=1; i<L.size(); i++)
	{
		string Q = WriteToString(GetRoss(), GetRoss()->Fields[L[i].m_FieldNo].m_Signats[L[i].GetSignatNo()].sFrmt, L[i]);
		// cмотри выше Положение о выравнивании
		if (L[i].m_LevelId > LevelId)
		{
			LevelId = L[i].m_LevelId;
			S.Format ("%i %s",LevelId, Q.c_str());

		}
		else
			if (LevelId > 0)
			{
				S = CString("  ")+Q.c_str();
			}
			else
				S = Q.c_str();

		CString R;
		R.Format ("%10s%s"," ",S);

		InsertLine (i+A.m_Fields[nPos].StartLine,R);
	}

	// StartLine не изменился, а EndLine нужно вычислить заново
	((CArticleDoc*)GetDocument())->Markout();
};

void CArticleView::EditFormulaValue (int nPos, char TypeRes)
{
	vector<TCortege> OldList; //старое значение
	vector<TCortege> NewList;
	const CTempArticle& A = ((CArticleDoc*)GetDocument())->m_Article;
	CRossDevTextField F(	A.m_Fields[nPos].FieldNo, A.m_Fields[nPos].LeafId, A.m_Fields[nPos].BracketLeafId);
	

	CArticleDoc* D =  (CArticleDoc*)GetDocument();
	try
	{
		D->AddCortegeToVector ( OldList,  F);
	}
	catch (...)
	{
   	    
	};


    if (TypeRes == frMany)
	{
		CSetEditor dlg (GetRossDoc(), OldList, NewList, F);
		if (dlg.DoModal() != IDOK) return;
	}
	else
	{
		CFormulaEditor dlg (GetRossDoc(), OldList, NewList, F);
		if (dlg.DoModal() != IDOK) return;
	};
	  

     WriteToEdit(NewList, nPos );
};


void CArticleView::EditValue(int nPos)
{
	const CTempArticle& A = ((CArticleDoc*)GetDocument())->m_Article;
	CRossDevTextField F(	A.m_Fields[nPos].FieldNo, A.m_Fields[nPos].LeafId, A.m_Fields[nPos].BracketLeafId);
	BYTE Type = GetRoss()->Fields[F.FieldNo].TypeRes;
	switch (Type) {
      case frOne     : EditOneValue (F); break;
	  case frMany    :
	  case frFormula : EditFormulaValue (nPos,GetRoss()->Fields[F.FieldNo].TypeRes); break;
	};
  
};

void CArticleView::OnArticleFormValue() 
{
	// TODO: Add your control notification handler code here
	try 
	{
		const CTempArticle& A = ((CArticleDoc*)GetDocument())->m_Article;

		if (!A.m_Fields.empty()) 
		{
			int nPos = m_FldScroll.GetScrollPos();
			if (nPos >= A.m_Fields.size())
				nPos = A.m_Fields.size() - 1;
			SetFocusToField (nPos);
			EditValue (nPos);
			SetFocusToField (nPos);
		};
	}
	catch (...)
	{
		AfxMessageBox ("Some error occured");
	};

} 



size_t CountLemmatize = 0;
void Lemmatize(char* OutBuf, char* InBuf) 
{
	TRACE (InBuf);
	TRACE ("\n");
	OutBuf[0] = 0;
	if ( !InBuf 
		|| (strlen(InBuf) == 0)) return;
	MorphLanguageEnum langua  =  morphRussian;
	if (is_english_alpha ((BYTE)InBuf[0]))
		langua = morphEnglish;
	strcpy (OutBuf, InBuf);
	const CLemmatizer* P = GetSemBuilder().m_RusStr.m_pData->GetLemmatizer(langua);
	if (!P) return;

	try 
	{
		CountLemmatize++;

		
		vector<CFormInfo> ParadigmCollection;
		string WordForm = InBuf;
		P->CreateParadigmCollection(false, WordForm, false,false, ParadigmCollection);
		if (ParadigmCollection.empty()) return;
		string t = ParadigmCollection[0].GetWordForm(0);
		strcpy(OutBuf, t.c_str());
		RusMakeLower(OutBuf);
	}
	catch (...)
	{

	};
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

bool IsKeyWordArticle (const CString& word, COLORREF& C, DWORD Data)
{
	CArticleView* V = (CArticleView*)Data;
	CRossDoc* RossDoc = V->GetRossDoc();
	
	
	TBaseDomItem I;
	if (word.GetLength() > EntryStrSize - 1) return false;
	strcpy (I.ItemStr,  (const char*)word);
	vector<TBaseDomItem>::const_iterator It = lower_bound (RossDoc->m_BasicDomItems.begin(), RossDoc->m_BasicDomItems.end(), I);
    if (    (It == RossDoc->m_BasicDomItems.end())
		|| !(*It == I)
		|| (  CString(RossDoc->GetRoss()->m_Domens[It->DomNo].DomStr) == CString("D_1"))) // если это не элемент метаязыка
	{
		if (IsRossEntry(word))
		{
			C = RGB(0,0,255);
			return true;
		};
	}
	else
	{
		
		BYTE no = RossDoc->m_DomParamsPtr[It->DomNo];
		if (no == ErrUChar) return 0;
		C = RossDoc->m_DomainParams[no].Color;
		return true;
	};

	StringVector::const_iterator field_it = lower_bound (RossDoc->m_Fields.begin(), RossDoc->m_Fields.end(), I.ItemStr);
	if (		(field_it != RossDoc->m_Fields.end())
			&&	(*field_it == I.ItemStr)
		)
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

	m_Colorizer.InitializeParser(IsKeyWordArticle, (DWORD)this);

	long mask = m_RichEdit.GetEventMask();
	m_RichEdit.SetEventMask(mask |= ENM_CHANGE );

}

void CArticleView::SetText(CString S)
{
	S.Replace("\n", "\r\n");
	m_RichEdit.SetWindowText(S);
	parse();
};


IMPLEMENT_DYNCREATE(CMyEditCtrl, CRichEditCtrl)
BEGIN_MESSAGE_MAP(CMyEditCtrl, CRichEditCtrl)
		ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

inline bool CanBeEntrySymbol(BYTE c)
{
	return		is_alpha(c) 
			||  c == '_'
			||	 c == '-';
};
void CMyEditCtrl::OnLButtonDblClk( UINT nFlags,  CPoint point )
{
	CArticleView* V = (CArticleView*)GetParent();
	int CharNo = CharFromPos(point);
	int LineNo = LineFromChar(CharNo);
	CString SourceText;
	GetWindowText(SourceText);
	SourceText.Replace("\r\n", "\n");
	int start = CharNo;
	for (; start >=0; start--)
		if (!CanBeEntrySymbol( (BYTE)SourceText.GetAt(start) ) )
			break;
	start++;
	if (start > CharNo) return;
	int end = CharNo;
	for (; end < SourceText.GetLength(); end++)
		if ( !CanBeEntrySymbol( (BYTE)SourceText.GetAt(end) ) )
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
  		WORD UnitNo = pDoc->GetRoss()->LocateUnit((const char*)Word, MeanNum);
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