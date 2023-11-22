#include "stdafx.h"
#include "ColorRichEditView.h"

//////////////////////////////////////////////////////////////////////////////
// CColorRichEditView class implementation
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CColorRichEditView, CRichEditView)

BEGIN_MESSAGE_MAP(CColorRichEditView, CRichEditView)
	//{{AFX_MSG_MAP(CSLFView)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

//----------------------------------------------------------------------------
CRichEditRedrawer::CRichEditRedrawer( CColorRichEditView& view )
: m_view(view)
{
	GetRichEditCtrl().GetSel(m_selBegin, m_selEnd);

	//turn off response to onchange events
	long mask = GetRichEditCtrl().GetEventMask();
	GetRichEditCtrl().SetEventMask(mask &= ~ENM_CHANGE );

	//std::set redraw to false to reduce flicker, and to speed things up
	GetRichEditCtrl().SetRedraw(FALSE);
}

//----------------------------------------------------------------------------
CRichEditRedrawer::~CRichEditRedrawer()
{
	//do some cleanup
	GetRichEditCtrl().SetSel(0,0);
	GetRichEditCtrl().SetRedraw(TRUE);
	GetRichEditCtrl().RedrawWindow();

	long mask = GetRichEditCtrl().GetEventMask();
	GetRichEditCtrl().SetEventMask(mask |= ENM_CHANGE );
	GetRichEditCtrl().SetSel(m_selBegin, m_selEnd);
}

//----------------------------------------------------------------------------
void CRichEditRedrawer::RedrawLine( int index, bool bModified )
{
	m_view.m_Colorizer.Colorize(&GetRichEditCtrl(),index,bModified);
}

//----------------------------------------------------------------------------
void CColorRichEditView::RedrawLine( int index, bool bModified )
{
	if( m_redrawInProcess ) return;

	m_redrawInProcess = true;
	CHARRANGE cr;
	GetRichEditCtrl().GetSel(cr);
	m_Colorizer.Colorize(&GetRichEditCtrl(),index,bModified);
	GetRichEditCtrl().SetSel(cr);
	m_redrawInProcess = false;
}

//----------------------------------------------------------------------------
void CColorRichEditView::RedrawLines( bool bModified )
{
	if( m_redrawInProcess ) return;
	m_redrawInProcess = true;
	{
		CRichEditRedrawer rd(*this);
		m_Colorizer.Colorize(&GetRichEditCtrl(),GetRichEditCtrl().GetLineCount(),bModified);
	}
	m_redrawInProcess = false;
}


void CColorRichEditView::SetAccentByIndex(int ind)
{
	CRichEditCtrl& re = GetRichEditCtrl();
	if( ind>=re.GetTextLength()-1 || ind<0 ) 
		return;
	CString Paradigm = GetText();
	int lineInd = re.LineIndex();
	int line_no = re.LineFromChar(lineInd);
	ind += line_no; // delete '\r'
	int wordStart = ind;
	{
		while (wordStart > 0 && !isspace((BYTE) Paradigm[wordStart]))
			wordStart--;
	};
	uint32_t wordEnd = ind;

	while (wordEnd < Paradigm.GetLength() && !isspace((BYTE)Paradigm.GetAt(wordEnd) ))
		wordEnd++;
	int accOld = Paradigm.Find(_T("'"), wordStart);
	if( accOld>=0  && accOld < wordEnd) 
	{
		if( accOld<ind ) --ind;
		Paradigm.Delete(accOld);
	}
	Paradigm.Insert(ind+1,_T("'"));
	SetText(Paradigm);
	
}


//----------------------------------------------------------------------------
// fill in accent 
//----------------------------------------------------------------------------
void CColorRichEditView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRichEditCtrl& re = GetRichEditCtrl();
	int ind = re.CharFromPos(point);
	SetAccentByIndex(ind);
}

void CColorRichEditView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();
	m_nWordWrap = CRichEditView::WrapNone;
	WrapChanged();
}

//----------------------------------------------------------------------------


void CColorRichEditView::SetAccentFromThisLineToTheEndofParadigm()
{
	CRichEditCtrl& re = GetRichEditCtrl();
	int lineInd = re.LineIndex();
	CString Paradigm = GetText();
	int line_no = re.LineFromChar(lineInd);
	lineInd += line_no; // delete '\r'
	uint32_t wordEnd = lineInd;
	while (wordEnd < Paradigm.GetLength() && !isspace((BYTE)Paradigm.GetAt(wordEnd) ))
		wordEnd++;
	int acc= Paradigm.Find(_T("'"), lineInd);
	if ( (acc <= 0) || (acc >=wordEnd)) return; 
	CString NewParadigm;
	auto lines = StringSplit(Paradigm, "\r\n");
	for (int i = 0; i < lines.size(); ++i)
	{
		int space_index = lines[i].FindOneOf(_T(" \t"));
		if ((i  <= line_no+1) || (space_index == -1))
		{
			NewParadigm += lines[i];
		}
		else
		{
			CString wrd = lines[i].Mid(0, space_index);
			int ind = wrd.Find(_T("'"));
			if (ind != -1) {
				wrd.Delete(ind, 1);
			}
			wrd.Insert(acc - lineInd, _T("'"));
			NewParadigm += wrd + CString(_T(" ")) + lines[i].Mid(space_index);
		};
		NewParadigm += CString("\r\n");
	};

	SetText(NewParadigm);
}

BOOL CColorRichEditView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
		if(pMsg->wParam==VK_F3)
		{
			try {
				SetAccentFromThisLineToTheEndofParadigm();
			}
			catch (...)
			{
				ErrorMessage ("An exception occurred");
			};

		};

	return CRichEditView::PreTranslateMessage(pMsg);
}

CString CColorRichEditView::GetText() const
{
	CString s;
	GetRichEditCtrl().GetWindowText(s);
	return s;
};

void  CColorRichEditView::SetText(const CString& S)
{
	GetRichEditCtrl().SetSel(0, -1);
	GetRichEditCtrl().Clear();

	GetRichEditCtrl().SetSel(-1, -1);
	GetRichEditCtrl().ReplaceSel(S);
};
