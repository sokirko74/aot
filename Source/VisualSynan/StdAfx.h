// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1C505B39_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
#define AFX_STDAFX_H__1C505B39_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0500

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#import "..\..\bin\SynAn.tlb"
using namespace SYNANLib;

#include "../common/utilit.h"

#pragma warning(disable: 4786)
#include <map>



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_STDAFX_H__1C505B39_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
