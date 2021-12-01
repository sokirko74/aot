#pragma once

#pragma warning(disable:4786)
#pragma warning(disable:4267)


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from windows headers
#endif
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "morph_dict/common/util_classes.h"
#include "../MorphWizardLib/wizard.h"
#include "HELPIDS.H"
#include <afxrich.h>

inline CString IntToStr(int w)
{
	CString s;
	s.Format(_T("%i"), w);
	return s;
}

inline std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

inline std::vector<CString> StringSplit(CString sFrom, CString delim) {
	int i = 0;
	std::vector<CString> saItems;
	for (CString sItem = sFrom.Tokenize(delim, i); i >= 0; sItem = sFrom.Tokenize(delim, i))
	{
		saItems.push_back(sItem);
	}
	return saItems;
}
