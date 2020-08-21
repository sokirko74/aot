// stdafx.cpp : source file that includes just the standard includes
//	VisualSynan.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "../common/COM/COMSyntaxHolder.h"
#include "VisualSynan.h"
#include <sstream>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

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

CString _OUT(BSTR t) {
	return  UTF8toUTF16((const char*)_bstr_t(t));
	//std::wstring_convert<std::codecvt_utf8_utf16<char16_t>> converter;
	//std::wstring_convert<std::codecvt<char16_t, char, std::mbstate_t>, char16_t> converter;
	//std::wstring w = converter.from_bytes(utf8str.c_str());
	///return CString(w.c_str());
	//return CString("");
}

CString _IN(CString s) {
	//std::string str = convert_to_utf8((const char*)s, ((CVisualSynanApp*)AfxGetApp())->m_SyntaxHolder.m_CurrentLanguage);
	//EnumFontFamiliesEx
	return s;
}

std::string WstrToUtf8Str(const std::wstring& wstr)
{
	std::string retStr;
	if (!wstr.empty())
	{
		int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

		if (sizeRequired > 0)
		{
			std::vector<char> utf8String(sizeRequired);
			int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
				-1, &utf8String[0], utf8String.size(), NULL,
				NULL);
			if (bytesConverted != 0)
			{
				retStr = &utf8String[0];
			}
			else
			{
				std::stringstream err;
				err << __FUNCTION__
					<< " std::string WstrToUtf8Str failed to convert wstring '"
					<< wstr.c_str() << L"'";
				throw std::runtime_error(err.str());
			}
		}
	}
	return retStr;
}
