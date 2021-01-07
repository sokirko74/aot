#ifdef WIN32
#include <afxwin.h>
#include <comutil.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <tchar.h>WstrToUtf8Str
#include <vector>
#include "windows_unicode.h"
#include "../utilit.h"

CStringW UTF8toUTF16(const CStringA& utf8)
{
	CStringW utf16;
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	if (len > 1)
	{
		wchar_t* ptr = utf16.GetBuffer(len - 1);
		if (ptr) MultiByteToWideChar(CP_UTF8, 0, utf8, -1, ptr, len);
		utf16.ReleaseBuffer();
	}
	return utf16;
}

CString ReadStrFromCOM(BSTR t) {
	return  UTF8toUTF16((const char*)_bstr_t(t));
}

std::string WstrToUtf8Str(CStringW wide_string) {
	std::wstring w(wide_string);
	return utf16_to_utf8(w);
}



#endif