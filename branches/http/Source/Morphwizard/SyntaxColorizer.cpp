// SyntaxColorizer.cpp: implementation of the CSyntaxColorizer class.
//
// Version:	1.0.0
// Author:	Jeff Schering jeffschering@hotmail.com
// Date:	Jan 2001
// Copyright 2001 by Jeff Schering
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SyntaxColorizer.h"

#define CLR_PLAIN					RGB(0,0,0)
#define CLR_BKGRD					RGB(255,255,255)
#define CLR_BKGRD_MODIFIED			RGB(210,200,220)
#define CLR_BKGRD_UNKNOWN_ACCENT	RGB(255,230,230)

#ifdef DETECT_MEMORY_LEAK
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSyntaxColorizer::CSyntaxColorizer()
{
	for (int i = 0; i < 256; i++)
		Alphas[i] = is_alpha(i) || isdigit(i) || (i == '_');

	ZeroMemory(&m_cfDefault,sizeof(m_cfDefault));

	m_cfDefault.dwMask = CFM_CHARSET | CFM_FACE | CFM_SIZE | CFM_OFFSET | CFM_COLOR 
		| CFM_BACKCOLOR;
/*
	m_cfDefault.dwMask ^= CFM_ITALIC ^ CFM_BOLD ^ CFM_STRIKEOUT ^ CFM_UNDERLINE
		^CFM_ANIMATION ^CFM_KERNING ^CFM_LCID ^CFM_REVAUTHOR ^CFM_SPACING ^CFM_STYLE
		^CFM_UNDERLINETYPE ^CFM_WEIGHT;
*/
	m_cfDefault.dwEffects = CFE_AUTOBACKCOLOR;

	m_cfDefault.yHeight = 200; //10pts * 20 twips/point = 200 twips
	m_cfDefault.bCharSet = ANSI_CHARSET;
	m_cfDefault.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
	m_cfDefault.yOffset = 0;
	m_cfDefault.crTextColor = CLR_PLAIN;
	strcpy(m_cfDefault.szFaceName,"Courier New");
	m_cfDefault.crBackColor = CLR_BKGRD;
	m_cfDefault.cbSize = sizeof(m_cfDefault);

}

void CSyntaxColorizer::InitializeParser(IsKeyWordFunction Func, void* UserData)
{
	IsKeyWord = Func;
	m_UserData = UserData;
}

void CSyntaxColorizer::Colorize( CRichEditCtrl *pCtrl, int lineIndex, bool bModified ) 
{
	CString SourceText;
	pCtrl->GetWindowText(SourceText);
	int k = SourceText.Replace("\r\n", "\n");
	
	long nStartChar=0, nEndChar=SourceText.GetLength();
	
	if( lineIndex>=-1 && lineIndex < pCtrl->GetLineCount() ) 
	{
		nStartChar = pCtrl->LineIndex(lineIndex);
		nEndChar = nStartChar + pCtrl->LineLength(lineIndex);
	}

	CHARFORMAT2 cf = m_cfDefault;
	if( bModified ) 
	{
		cf.crBackColor = CLR_BKGRD_MODIFIED;
		cf.dwEffects = 0;
	}

	pCtrl->SetSel(nStartChar,nEndChar);
	pCtrl->SetSelectionCharFormat(cf);

	cf = m_cfDefault;
	cf.dwMask = CFM_BOLD;
	cf.crBackColor = CLR_BKGRD_UNKNOWN_ACCENT;
	cf.dwEffects = CFE_BOLD;

	for( int wordStart=nStartChar; wordStart<nEndChar; )
	{
		int lineEnd = SourceText.Find("\n",wordStart);
		if( lineEnd==-1 || lineEnd>nEndChar ) lineEnd=nEndChar;
		// пропускаем пробелы в начале строки
		for( ; wordStart<lineEnd && SourceText[wordStart]==' '; ++wordStart );
		if( wordStart<lineEnd ) 
		{
			// ищем ударение
			bool hasAccent=false;
			int i=wordStart;
			for( ; i<lineEnd && SourceText[i]!=' '; ++i )
			{	
				if( SourceText[i]=='\'' ) hasAccent=true;
			}

			if( !hasAccent ) 
				cf.dwMask |= CFM_BACKCOLOR;
			else
				cf.dwMask &= ~CFM_BACKCOLOR;

			pCtrl->SetSel(wordStart, i);
			pCtrl->SetSelectionCharFormat(cf);
		}
		wordStart = lineEnd + 1;
	}

	cf = m_cfDefault;
	cf.dwMask = CFM_BOLD | CFM_COLOR;
	cf.dwEffects = 0;

	const size_t MaxKeyWordLen = 50;
	char buffer[MaxKeyWordLen+1];

	for (long x = nStartChar; x < nEndChar; x++)
	{
		long end = x;
		for (; ((end - x) < MaxKeyWordLen) &&  Alphas[(unsigned char) SourceText.GetAt(end) ]; end++)
			buffer[end - x] = SourceText.GetAt(end);

		if (end == x) 
			continue;

		if ( (end - x) >= MaxKeyWordLen)
		{
			while (Alphas[(unsigned char) SourceText.GetAt(end) ])
				end++;
		}
		else
		{
			buffer[end-x] = 0;
			if (IsKeyWord(buffer, cf.crTextColor, m_UserData))
			{
				pCtrl->SetSel(x, end);
				pCtrl->SetSelectionCharFormat(cf);
			};
		};
		x = end;
	}
}



