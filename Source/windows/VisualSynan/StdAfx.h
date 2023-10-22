#pragma once

#define _WIN32_WINNT 0x0600

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include "morph_dict/common/utilit.h"
#include "windows/common/windows_unicode.h"
#include <tchar.h>
#include <map>

extern CString FromRMLEncode(std::string s);
