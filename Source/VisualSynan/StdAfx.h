#pragma once

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#import "../SynAn/SynAn.tlb"
#import "../Lemmatizer/Lemmatizer.tlb"
#import "../graphan/graphan.tlb"
#include "../common/utilit.h"
#include <tchar.h>
#include <map>
extern CString _OUT(BSTR t);
extern CString _IN(CString s);
extern std::string WstrToUtf8Str(const std::wstring& wstr);
