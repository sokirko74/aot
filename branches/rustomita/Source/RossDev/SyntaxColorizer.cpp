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

#define CLR_PLAIN	RGB(0,0,0)

#ifdef _DEBUG
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

	m_cfDefault.dwMask = CFM_CHARSET | CFM_FACE | CFM_SIZE | CFM_OFFSET | CFM_COLOR;
	m_cfDefault.dwMask ^= CFM_ITALIC ^ CFM_BOLD ^ CFM_STRIKEOUT ^ CFM_UNDERLINE;
	m_cfDefault.dwEffects = 0;
	m_cfDefault.yHeight = 200; //10pts * 20 twips/point = 200 twips
	m_cfDefault.bCharSet = ANSI_CHARSET;
	m_cfDefault.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
	m_cfDefault.yOffset = 0;
	m_cfDefault.crTextColor = CLR_PLAIN;
	strcpy(m_cfDefault.szFaceName,"Courier New");
	m_cfDefault.cbSize = sizeof(m_cfDefault);

}



void CSyntaxColorizer::InitializeParser(IsKeyWordFunction Func, DWORD UserData)
{
	IsKeyWord = Func;
	m_UserData = UserData;
}


void CSyntaxColorizer::Colorize(CRichEditCtrl *pCtrl, long nStartChar, long nEndChar) 
{
	CString SourceText;
	pCtrl->GetWindowText(SourceText);
	SourceText.Replace("\r\n", "\n");
	pCtrl->SetSel(nStartChar,nEndChar);
	pCtrl->SetSelectionCharFormat(m_cfDefault);


	CHARFORMAT cf = m_cfDefault;
	cf.dwMask = CFM_BOLD | CFM_COLOR;
	cf.dwEffects = 0;


	const size_t MaxKeyWordLen = 50;
	char buffer[MaxKeyWordLen+1];

	long SourceLength = SourceText.GetLength();
	if (nEndChar == -1) 
		nEndChar = SourceLength;
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



