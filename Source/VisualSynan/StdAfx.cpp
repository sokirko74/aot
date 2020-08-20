// stdafx.cpp : source file that includes just the standard includes
//	VisualSynan.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "../common/COM/COMSyntaxHolder.h"
#include "VisualSynan.h"

CString _OUT(BSTR t) {
	const CCOMSyntaxHolder& Holder = ((CVisualSynanApp*)AfxGetApp())->m_SyntaxHolder;
	std::string s  = (const char*)_bstr_t(t);
	s = convert_from_utf8(s.c_str(), Holder.m_CurrentLanguage);
	return CString(s.c_str());
}

CString _IN(CString s) {
	std::string str = convert_to_utf8((const char*)s, ((CVisualSynanApp*)AfxGetApp())->m_SyntaxHolder.m_CurrentLanguage);
	return str.c_str();
}
