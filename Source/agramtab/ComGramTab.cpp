#include "stdafx.h"
#include <comdef.h>
#include "ComGramTab.h"
#include "../AgramtabLib/RusGramTab.h"     
#include "../AgramtabLib/GerGramTab.h"     
#include "../AgramtabLib/EngGramTab.h"     


CComGramTab::CComGramTab()
{
};


CComGramTab::~CComGramTab()
{
	assert (m_pAgramtab);
	delete m_pAgramtab;
};

std::string CComGramTab::_IN(BSTR str)
{
	std::string s = (const char*)_bstr_t(str);
	return convert_from_utf8(s.c_str(), m_pAgramtab->m_Language);
}

BSTR CComGramTab::_OUT(std::string& str)
{
	std::string s = convert_to_utf8(str, m_pAgramtab->m_Language);
	return _bstr_t(s.c_str()).copy();
}


STDMETHODIMP CComGramTab::GetPartOfSpeech(BSTR gram_code, BYTE* result)
{
	*result = m_pAgramtab->GetPartOfSpeech(_IN(gram_code).c_str());

	if (*result == UnknownPartOfSpeech)
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP CComGramTab::GetGrammems(BSTR gram_code, unsigned hyper *result)
{
	uint64_t r;
	m_pAgramtab->GetGrammems(_IN(gram_code).c_str(), r);
	*result = r;
	return S_OK;
}


STDMETHODIMP CComGramTab::GrammemsToStr(uint64_t grammems, BSTR *result)
{
	char szGrammems[32*5];
	m_pAgramtab->grammems_to_str(grammems, szGrammems);
	*result = _OUT(std::string(szGrammems));
	return S_OK;
}

STDMETHODIMP CComGramTab::GetPartOfSpeechStr(BYTE PartOfSpeech, BSTR *result)
{	
	std::string s = m_pAgramtab->GetPartOfSpeechStr(PartOfSpeech);
	s = convert_to_utf8(s, m_pAgramtab->m_Language);
	*result =  _bstr_t(s.c_str()).copy(); 
	return S_OK;
}

STDMETHODIMP CComGramTab::Load()
{
	if (!m_pAgramtab->LoadFromRegistry()) return E_FAIL;
	return S_OK;
}

STDMETHODIMP CComGramTab::HaveEqualPartOfSpeech(BSTR gram_code1, BSTR gram_code2, BOOL* result)
{
	*result = m_pAgramtab->AreEqualPartOfSpeech(_IN(gram_code1).c_str(), _IN(gram_code2).c_str());
	return S_OK;
}

STDMETHODIMP CComGramTab::ProcessPOSAndGrammems(BSTR in, BYTE *PartOfSpeech, uint64_t *Grammems)
{
	if (!m_pAgramtab->ProcessPOSAndGrammems (_IN(in).c_str(), *PartOfSpeech, *Grammems))
		return E_FAIL;

	if (*PartOfSpeech == UnknownPartOfSpeech)
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP CComGramTab::GleicheGenderNumberCase(BSTR common_noun_gram_code, BSTR noun_gram_code, BSTR adj_code, BOOL *result)
{
	*result = m_pAgramtab->GleicheGenderNumberCase(
		_IN(common_noun_gram_code).c_str(), 
		_IN(noun_gram_code).c_str(),
		_IN(adj_code).c_str()) ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComGramTab::GleicheCaseNumber(BSTR gram_code1, BSTR gram_code2, BOOL *result)
{
	*result = m_pAgramtab->GleicheCaseNumber(
		_IN(gram_code1).c_str(), 
		_IN(gram_code2).c_str()) ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComGramTab::GleicheGenderNumber(BSTR gram_code1, BSTR gram_code2, BOOL *result)
{
	*result = m_pAgramtab->GleicheGenderNumber(
		_IN(gram_code1).c_str(), 
		_IN(gram_code2).c_str()) ? TRUE : FALSE;
	return S_OK;
}



STDMETHODIMP CComGramTab::GleicheSubjectPredicate(BSTR gram_code1, BSTR gram_code2, BOOL *bRes)
{
	*bRes = m_pAgramtab->GleicheSubjectPredicate(
		_IN(gram_code1).c_str(), 
		_IN(gram_code2).c_str() ) ? TRUE : FALSE;
	return S_OK;
}



STDMETHODIMP CComGramTab::GetAllGrammems(BSTR gram_code, uint64_t *result)
{
	*result = m_pAgramtab->GetAllGrammems(_IN(gram_code).c_str());
	return S_OK;
}




STDMETHODIMP CComGramTab::FindGrammem(BSTR gram_codes, uint64_t grammems, BOOL *result)
{
	*result = m_pAgramtab->FindGrammems(_IN(gram_codes).c_str(), grammems) ? TRUE : FALSE;
	return S_OK;
}



STDMETHODIMP CComGramTab::ProcessPOSAndGrammemsIfCan(BSTR in, BYTE *PartOfSpeech, unsigned hyper *Grammems, BOOL* Result)
{
	*Result = m_pAgramtab->ProcessPOSAndGrammems (
		_IN(in).c_str(), *PartOfSpeech, *Grammems) ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComGramTab::GetClauseTypeByName(BSTR bstrTypeName, long *lType)
{
	*lType = m_pAgramtab->GetClauseTypeByName(_IN(bstrTypeName).c_str());
	if (*lType == -1)
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CComGramTab::GetClauseTypeByNameIfCan(BSTR bstrTypeName, long *lType, BOOL *Result)
{
	try
	{
		*Result = (GetClauseTypeByName (bstrTypeName, lType) != E_FAIL);
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CComGramTab::GetClauseNameByType(long type, BSTR *bstrName)
{
	try
	{
		*bstrName = _OUT(std::string(m_pAgramtab->GetClauseNameByType(type)));	
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CComGramTab::GetGramCodeByGrammemsAndPartofSpeechIfCan(BYTE Pos, uint64_t grammems, BSTR *gramcodes, BOOL* bRes)
{
	std::string str;
	*bRes = m_pAgramtab->GetGramCodeByGrammemsAndPartofSpeechIfCan(Pos, grammems, str);
	*gramcodes = _OUT(str);

	return S_OK;
}

STDMETHODIMP CComGramTab::get_PartOfSpeechCount(BYTE *pVal)
{
	*pVal = m_pAgramtab->GetPartOfSpeechesCount();
	return S_OK;
}


STDMETHODIMP CComGramTab::get_GrammemsCount(LONG *pVal)
{
	*pVal = (LONG)m_pAgramtab->GetGrammemsCount();
	return S_OK;
}


CComRusGramTab::CComRusGramTab() : CComGramTab()
{
	m_pAgramtab  = new CRusGramTab();
	assert (m_pAgramtab);
};

CComGerGramTab::CComGerGramTab() : CComGramTab()
{
	m_pAgramtab  = new CGerGramTab();
	assert (m_pAgramtab);
};


CComEngGramTab::CComEngGramTab() : CComGramTab()
{
	m_pAgramtab  = new CEngGramTab();
	assert (m_pAgramtab);
};


