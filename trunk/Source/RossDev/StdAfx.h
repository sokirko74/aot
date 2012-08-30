// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(rosssdev_stadafx)
#define rosssdev_stadafx

#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0501           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning (disable : 4786)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include "resource.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include <afxcmn.h>			// MFC support for Windows Common Controls


#include "..\common\cortege.h"

const int MaxNumDom = 10;


struct CRossDevTextField
{
	WORD  StartPosition;
	WORD  EndPosition;
	DWORD StartLine;
	DWORD EndLine;

	BYTE  FieldNo;
	BYTE  LeafId;
	BYTE  BracketLeafId;
	CRossDevTextField (BYTE _FieldNo, BYTE _LeafId, BYTE _BracketLeafId)
	{
		LeafId = _LeafId;
		BracketLeafId = _BracketLeafId;
		FieldNo = _FieldNo;
	};

	bool operator==(const CRossDevTextField& X) const
		{return    (FieldNo == X.FieldNo)
				&& (BracketLeafId  == X.BracketLeafId)
				&& (LeafId  == X.LeafId); }

};

inline bool SaveToFile (const CRichEditCtrl& C,  CString FileName ) 
{
	FILE * fp = fopen (FileName,"wb");
	if (!fp) return false;
	CString S;
	C.GetWindowText(S);
	fprintf (fp, "%s", (const char*)S);
	fclose(fp);
	return true;
};

inline int GetVisibleLinesCount (const CRichEditCtrl& C) 
{
	CRect R;
	C.GetWindowRect(&R);
	CHARFORMAT cf;
	C.GetDefaultCharFormat(cf);
	HDC lngDC = ::GetDC(HWND_DESKTOP);
	double TwipsPerPixelX = 1440 / GetDeviceCaps(lngDC, LOGPIXELSX);
	int LineInPixel = (cf.yHeight + cf.yOffset)/TwipsPerPixelX;
	return R.Height() / LineInPixel;
};

inline bool LoadFromFile (CRichEditCtrl& C,  CString FileName )
{
	FILE * fp = fopen (FileName,"rb");
	if (!fp) return false;
	char buffer[1000];
	CString S;
	while (fgets(buffer, 1000, fp))
		S += buffer;
	fclose(fp);	
	
	C.SetWindowText(S);
	return true;
}

inline void SelectLine ( CRichEditCtrl& C, long LineNo )
{
	long nStartChar = C.LineIndex(LineNo);
	long nEndChar = nStartChar + C.LineLength(); 
	C.SetSel(nStartChar, nEndChar);
};



#include "../common/utilit.h"
#include "../SemanLib/translatorholder.h"
#include "../SemanLib/rossholder.h"
#include "rossdev.h"


#endif 
