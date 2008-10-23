// GraphmatFile.cpp : Implementation of CComGraphmatFile
#include "StdPch.h"
#include "Graphan.h"

#include "ComGraphmatFile.h"
#include <comdef.h>
#include "../StructDict/ComDictionary.h"


STDMETHODIMP CComGraphmatFile::FreeTable()
{
	CGraphmatFile::FreeTable();
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::HasDescr(UINT LineNo, UINT D, BOOL* result)
{
	*result = (CGraphmatFile::HasDescr (LineNo, D)) ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::GetLineCount(UINT *Count)
{
	*Count  = GetUnits().size();
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::GetWord(UINT LineNo, BSTR* s)
{
    try {
		string buf(GetUnits()[LineNo].GetToken(), GetUnits()[LineNo].GetTokenLength());
		*s =  _bstr_t(buf.c_str()).copy();
		return S_OK;
	}
	catch (...)
	{
	  return E_FAIL;
	};
}



extern bool GetDescriptorStr(int DescriptorNo, string& Result);

STDMETHODIMP CComGraphmatFile::GetDescriptorStr(UINT DescriptorNo, BSTR *result)
{
	string S;
	if (!::GetDescriptorStr(DescriptorNo,S))
		return E_FAIL;
	*result =  _bstr_t(S.c_str()).copy();
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::GetRubiconTypeStr(UINT RubiconTypeNo, BSTR *result)
{
	// TODO: Add your implementation code here
    try { 

	*result =  _bstr_t(GetStrByRubiconType((RubiconEnum)RubiconTypeNo)).copy();
 
	return S_OK;
	}
	catch (...)
	{
	  return E_FAIL;
	};
}

STDMETHODIMP CComGraphmatFile::GetLine(UINT LineNo, BSTR *result)
{
	char s[CriticalGraphemLineLength];
	CGraphmatFile::GetGraphematicalLine(s, LineNo);
	*result =  _bstr_t(s).copy();
	return S_OK;
}





STDMETHODIMP CComGraphmatFile::get_GraOutputFile(BSTR *pVal)
{
	*pVal = _bstr_t(m_GraOutputFile.c_str()).copy();
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::put_GraOutputFile(BSTR newVal)
{
	m_GraOutputFile = (const char*)_bstr_t(newVal);
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::GetOborotNo(UINT LineNo, LONG *result)
{
	short OborotNo = CGraphmatFile::GetOborotNo(LineNo);
	if (OborotNo == -1)
		return 0xffff;
	else
		m_pDicts->m_Oborottos[OborotNo ].m_UnitNo;
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::get_IsTableLoaded(BOOL *pVal)
{
     *pVal = (GetUnits().size() != 0 )? TRUE : FALSE;
	return S_OK;
}



STDMETHODIMP CComGraphmatFile::FreeDicts()
{
	CGraphmatFile::FreeDicts();
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::SetOborDic(IUnknown *Dic)
{
	assert (m_pDicts);
	const_cast<CGraphanDicts*>(m_pDicts)->SetOborDic(reinterpret_cast<CComDictionary*>(Dic));
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::get_bMacSynHierarchy(BOOL *pVal)
{
	*pVal = m_bMacSynHierarchy  ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::put_bMacSynHierarchy(BOOL newVal)
{
	m_bMacSynHierarchy  = newVal? true : false;
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::GetUnitOffset(int LineNo, int *Offset)
{
	*Offset = CGraphmatFile::GetTokenInputOffset(LineNo);  
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::GetUnitLength(int LineNo, BYTE *Length)
{
	*Length = GetUnits()[LineNo].GetTokenLength();  
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::LoadStringToGraphan(BSTR szBuffer)
{
	_bstr_t t = szBuffer;
	return CGraphmatFile::LoadStringToGraphan((const char*)t) ? S_OK : E_FAIL;
};


STDMETHODIMP CComGraphmatFile::LoadFileToGraphan(BSTR CommandLine)
{
	_bstr_t t = CommandLine;
	return CGraphmatFile::LoadFileToGraphan((const char*)t) ? S_OK : E_FAIL;
};

extern bool CheckPersonalLicense();
STDMETHODIMP CComGraphmatFile::LoadDicts()
{
	if (!CheckPersonalLicense())
		if (!CheckEvaluationTime()) 
			return false;
	return CGraphmatFile::LoadDicts() ? S_OK : E_FAIL;
};

STDMETHODIMP CComGraphmatFile::get_XmlMacSynOutputFile(BSTR *pVal)
{
	*pVal = _bstr_t(m_XmlMacSynOutputFile.c_str()).copy(); ;
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::put_XmlMacSynOutputFile(BSTR newVal)
{
	m_XmlMacSynOutputFile = (const char*)_bstr_t(newVal);
	return S_OK;
}



STDMETHODIMP CComGraphmatFile::get_Language(int *pVal)
{
	*pVal = m_Language;
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::put_Language(int newVal)
{
	m_Language = (MorphLanguageEnum)newVal ;
	return S_OK;
}


STDMETHODIMP CComGraphmatFile::get_bEmptyLineIsSentenceDelim(BOOL *pVal)
{
	*pVal =  m_bEmptyLineIsSentenceDelim? TRUE:FALSE;
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::put_bEmptyLineIsSentenceDelim(BOOL newVal)
{
	m_bEmptyLineIsSentenceDelim =  newVal? true:false;

	return S_OK;
}

STDMETHODIMP CComGraphmatFile::get_bUseParagraphTagToDivide(BOOL *pVal)
{
	*pVal =  m_bUseParagraphTagToDivide? TRUE:FALSE;
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::put_bUseParagraphTagToDivide(BOOL newVal)
{
	m_bUseParagraphTagToDivide =  newVal? true:false;

	return S_OK;
}

STDMETHODIMP CComGraphmatFile::get_bUseIndention(BOOL *pVal)
{
	*pVal =  m_bUseIndention? TRUE:FALSE;
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::put_bUseIndention(BOOL newVal)
{
	m_bUseIndention =  newVal? true:false;

	return S_OK;
}




STDMETHODIMP CComGraphmatFile::get_bSubdueWrongMacroSynUnitToMainRoot(BOOL* pVal)
{
	*pVal =  m_bSubdueWrongMacroSynUnitToMainRoot ? TRUE:FALSE;
	return S_OK;
}

STDMETHODIMP CComGraphmatFile::put_bSubdueWrongMacroSynUnitToMainRoot(BOOL newVal)
{
	m_bSubdueWrongMacroSynUnitToMainRoot =  newVal? true:false;

	return S_OK;
}

STDMETHODIMP CComGraphmatFile::GetTokenLanguage(int LineNo, int* Language)
{
	*Language = CGraphmatFile::GetTokenLanguage(LineNo);
	return S_OK;
}


