#pragma once

//  a header for windows MFC Unicode projects only (VisualSynan and demo.exe)

#include "utilit.h"
#include <afxwin.h>
#include <comutil.h>


inline CStringW UTF8toUTF16(const CStringA& utf8) {
	std::wstring w = utf8_to_utf16(std::string(utf8));
	return w.c_str();
}

inline CString ReadStrFromCOM(BSTR t) {
	return  UTF8toUTF16((const char*)_bstr_t(t));
}

inline std::string WstrToUtf8Str(CStringW wide_string) {
	std::wstring w(wide_string);
	return utf16_to_utf8(w);
}
