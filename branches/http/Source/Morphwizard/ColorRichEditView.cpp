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

	//set redraw to false to reduce flicker, and to speed things up
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
	if(!is_lower_vowel(Paradigm[ind], m_morphWizard.m_Language) )
		return;
	int wordStart = ind;
	{
		while (wordStart > 0 && !isspace((BYTE) Paradigm[wordStart]))
			wordStart--;
	};
	DWORD wordEnd = ind;

	while (wordEnd < Paradigm.GetLength() && !isspace((BYTE)Paradigm.GetAt(wordEnd) ))
		wordEnd++;
	int accOld = Paradigm.Find("'",wordStart);
	if( accOld>=0  && accOld < wordEnd) 
	{
		if( accOld<ind ) --ind;
		Paradigm.Delete(accOld);
	}
	Paradigm.Insert(ind+1,"'");
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
	DWORD wordEnd = lineInd;
	while (wordEnd < Paradigm.GetLength() && !isspace((BYTE)Paradigm.GetAt(wordEnd) ))
		wordEnd++;
	int acc= Paradigm.Find("'",lineInd);
	if ( (acc <= 0) || (acc >=wordEnd)) return; 
	StringTokenizer tok((const char*)Paradigm, "\r\n");
	int curr_line_no = 0;
	CString NewParadigm;
	while (tok())
	{
		curr_line_no++;
		if (curr_line_no  <= line_no+1) 
		{
			NewParadigm += tok.val();
			NewParadigm += "\r\n";
			continue;
		}
		else
		{
			StringTokenizer tk(tok.val(), " \t\r\n");
			if (tk())
			{
				string wrd = tk.val();
				int ind = wrd.find("'");
				if (ind != string::npos)
					wrd.erase(ind,1);
				wrd.insert(acc-lineInd, "'");
				NewParadigm += CString(wrd.c_str())+CString(" ")+CString(tk.get_rest())+CString("\r\n");
			}
			else
				NewParadigm += tok.val()+CString("\r\n");
		};
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

struct CStringAndPosition
{
	char*	m_pBuffer;
	int		m_BufferLength;
	int		m_Position;
	CStringAndPosition()
	{
		m_Position =0;
		m_pBuffer =0;

	};
	~CStringAndPosition()
	{
		if (m_pBuffer)
			delete m_pBuffer;
	};
};

/*
struct CStringAndPosition
{
	CString m_String;
	int m_Position;
};

static DWORD CALLBACK MyStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
   CString* pString = (CString*) dwCookie;
   for (size_t  i=0; i < cb; i+=2)
	*pString += pbBuff[i];
   *pcb = cb;
   return 0;
}

static DWORD CALLBACK MyStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
   CStringAndPosition* pString = (CStringAndPosition*) dwCookie;
   size_t  Char_no = 0;
   assert((cb%2) == 0);		


   while ((Char_no < cb)&& (pString->m_Position < pString->m_String.GetLength()))
   {
		pbBuff[Char_no] = pString->m_String.GetAt(pString->m_Position);
		Char_no++;
		pbBuff[Char_no] = 0;
		Char_no++;
		pString->m_Position++;
   };
   *pcb = Char_no;

   return 0;
}
*/

static DWORD CALLBACK MyStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CStringAndPosition* pString = (CStringAndPosition*) dwCookie;
	for (size_t  i=0; i < cb; i++)
	{
		pString->m_pBuffer[pString->m_Position++] = pbBuff[i];
	};
	*pcb = cb;
	return 0;
}

static DWORD CALLBACK MyStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
   CStringAndPosition* pString = (CStringAndPosition*) dwCookie;
   size_t  Char_no = 0;
   
   while ((Char_no < cb)&& (pString->m_Position < pString->m_BufferLength))
   {
		pbBuff[Char_no] = pString->m_pBuffer[pString->m_Position];
		Char_no++;
		pString->m_Position++;
   };
   *pcb = Char_no;

   return 0;
}

UINT	CColorRichEditView::GetCodePage() const
{
	if (m_morphWizard.m_Language == morphRussian)
		return 1251;
	else
		return 1252;
};
// converting diacritic marks
CString CColorRichEditView::GetText() const
{
	CRichEditCtrl& re = GetRichEditCtrl();
	int TextLen = re.GetTextLength();
	CStringAndPosition SP;
	SP.m_pBuffer = new char[TextLen*2];
	EDITSTREAM es;
	es.dwCookie = (DWORD) &SP;
	es.pfnCallback = MyStreamOutCallback; 
	GetRichEditCtrl().StreamOut(SF_TEXT|SF_UNICODE, es);
	char* buf = new char[TextLen+1];
	if (!WideCharToMultiByte(GetCodePage(), 0, (WCHAR*)SP.m_pBuffer, TextLen, buf, TextLen, NULL, NULL))
	{
		ErrorMessage ("WideCharToMultiByte failed");
	};
	buf[TextLen] =0;
	CString S = buf;
	delete buf;
	return S;
};


// converting diacritic marks
void  CColorRichEditView::SetText(const CString& S)
{
	CStringAndPosition SP;
	SP.m_BufferLength = MultiByteToWideChar(GetCodePage(), 0,(const char*)S, S.GetLength(), NULL, NULL);
	SP.m_pBuffer = new char[SP.m_BufferLength*2+1];
	memset(SP.m_pBuffer,0, SP.m_BufferLength*2+1);
	int debug = S.GetLength();
	assert (debug  == SP.m_BufferLength);
	if (S.GetLength()>0)
	{
		if (!MultiByteToWideChar(GetCodePage(), 0, (const char*)S, S.GetLength(), (WCHAR*)SP.m_pBuffer, SP.m_BufferLength))
		{
			ErrorMessage ("MultiByteToWideChar failed");
		};
	}
	
				

	SP.m_Position = 0;
	EDITSTREAM es;
	es.dwCookie = (DWORD) &SP;
	es.pfnCallback = MyStreamInCallback; 
	SP.m_BufferLength *= 2;
	GetRichEditCtrl().StreamIn(SF_TEXT|SF_UNICODE, es);
};
