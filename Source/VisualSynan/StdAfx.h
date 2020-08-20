#pragma once

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#define _WIN32_WINNT 0x0500

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#import "../SynAn/SynAn.tlb"

#include "../common/utilit.h"

#include <map>
extern CString _OUT(BSTR t);
extern CString _IN(CString s);
