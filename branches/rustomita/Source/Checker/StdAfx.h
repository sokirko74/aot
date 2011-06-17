// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__506BF518_3AE1_11D5_92A5_005004035BFC__INCLUDED_)
#define AFX_STDAFX_H__506BF518_3AE1_11D5_92A5_005004035BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0500
//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#import "../../bin/seman.tlb"
#import "../../bin/Synan.tlb"
using namespace SYNANLib;


#endif // !defined(AFX_STDAFX_H__506BF518_3AE1_11D5_92A5_005004035BFC__INCLUDED_)
