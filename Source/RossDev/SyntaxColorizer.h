// SyntaxColorizer.h: interface for the CSyntaxColorizer class.
//
// Version:	1.0.0
// Author:	Jeff Schering jeffschering@hotmail.com
// Date:	Jan 2001
// Copyright 2001 by Jeff Schering
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNTAXCOLORIZER_H__5C50E421_E4AC_11D4_A61E_60A459C10100__INCLUDED_)
#define AFX_SYNTAXCOLORIZER_H__5C50E421_E4AC_11D4_A61E_60A459C10100__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef bool (*IsKeyWordFunction)(const CString& word, COLORREF& C, DWORD Data);

class CSyntaxColorizer  
{
public:
	CSyntaxColorizer();
	void Colorize(CRichEditCtrl *pCtrl, long StartChar, long nEndChar);
	void InitializeParser(IsKeyWordFunction Func, DWORD UserData);


protected:
	CHARFORMAT m_cfDefault;
	bool Alphas[256];
	DWORD		m_UserData;
	IsKeyWordFunction IsKeyWord;
};

#endif // !defined(AFX_SYNTAXCOLORIZER_H__5C50E421_E4AC_11D4_A61E_60A459C10100__INCLUDED_)

