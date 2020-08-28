#pragma once

//  a header for Windows MFC Unicode projects only (VisualSynan and demo.exe)
#ifdef WIN32
#include <afxwin.h>

extern CStringW UTF8toUTF16(const CStringA& utf8);
extern CString ReadStrFromCOM(BSTR t);
extern std::string WstrToUtf8Str(const std::wstring& wstr);

#endif