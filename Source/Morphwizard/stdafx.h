#pragma once

#pragma warning(disable:4786)
#pragma warning(disable:4267)


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "../common/util_classes.h"
#include "../MorphWizardLib/wizard.h"
#include "helpids.h"
#include <afxrich.h>

inline CString IntToStr(int w)
{
	CString s;
	s.Format(_T("%i"), w);
	return s;
}
