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


STDMETHODIMP CComGramTab::GetPartOfSpeech(BSTR gram_code, BYTE* result)
{
	*result = m_pAgramtab->GetPartOfSpeech((const char*)_bstr_t (gram_code));

	if (*result == UnknownPartOfSpeech)
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP CComGramTab::GetGrammems(BSTR gram_code, QWORD *result)
{
	_bstr_t t = gram_code;
    m_pAgramtab->GetGrammems((const char*)t, *result);
	return S_OK;
}


STDMETHODIMP CComGramTab::GrammemsToStr(QWORD grammems, BSTR *result)
{
	char szGrammems[32*5];
	m_pAgramtab->grammems_to_str(grammems, szGrammems);
	CComBSTR bstrString(szGrammems);
	bstrString.CopyTo(result);
	//*result =  _bstr_t("test").copy(); 
	return S_OK;
}

STDMETHODIMP CComGramTab::GetPartOfSpeechStr(BYTE PartOfSpeech, BSTR *result)
{
	*result =  _bstr_t(m_pAgramtab->GetPartOfSpeechStr(PartOfSpeech)).copy(); 
	return S_OK;
}



STDMETHODIMP CComGramTab::Load()
{
	if (!m_pAgramtab->LoadFromRegistry()) return E_FAIL;
	return S_OK;
}

STDMETHODIMP CComGramTab::HaveEqualPartOfSpeech(BSTR gram_code1, BSTR gram_code2, BOOL* result)
{
	_bstr_t g1 = gram_code1;
	_bstr_t g2 = gram_code2;
	*result = m_pAgramtab->AreEqualPartOfSpeech(g1, g2);
	return S_OK;
}

STDMETHODIMP CComGramTab::ProcessPOSAndGrammems(BSTR in, BYTE *PartOfSpeech, QWORD *Grammems)
{
	_bstr_t g1 = in;
	if (!m_pAgramtab->ProcessPOSAndGrammems ((char*)g1, *PartOfSpeech, *Grammems))
	 return E_FAIL;

	if (*PartOfSpeech == UnknownPartOfSpeech)
	 return E_FAIL;

	return S_OK;
}


STDMETHODIMP CComGramTab::GleicheGenderNumberCase(BSTR common_noun_gram_code, BSTR noun_gram_code, BSTR adj_code, BOOL *result)
{
	*result = m_pAgramtab->GleicheGenderNumberCase((const char*)_bstr_t(common_noun_gram_code), (const char*)_bstr_t(noun_gram_code), (const char*)_bstr_t(adj_code)) ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComGramTab::GleicheCaseNumber(BSTR gram_code1, BSTR gram_code2, BOOL *result)
{
	*result = m_pAgramtab->GleicheCaseNumber((const char*)_bstr_t(gram_code1), (const char*)_bstr_t(gram_code2)) ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComGramTab::GleicheGenderNumber(BSTR gram_code1, BSTR gram_code2, BOOL *result)
{
	*result = m_pAgramtab->GleicheGenderNumber((const char*)_bstr_t(gram_code1), (const char*)_bstr_t(gram_code2)) ? TRUE : FALSE;
	return S_OK;
}



STDMETHODIMP CComGramTab::GleicheSubjectPredicate(BSTR gram_code1, BSTR gram_code2, BOOL *bRes)
{
	*bRes =m_pAgramtab->GleicheSubjectPredicate((const char*)_bstr_t(gram_code1), (const char*)_bstr_t(gram_code2)) ? TRUE : FALSE;
	return S_OK;
}



STDMETHODIMP CComGramTab::GetAllGrammems(BSTR gram_code, QWORD *result)
{
	*result = m_pAgramtab->GetAllGrammems((const char*)_bstr_t(gram_code));
	return S_OK;
}




STDMETHODIMP CComGramTab::FindGrammem(BSTR gram_codes, QWORD grammems, BOOL *result)
{
    _bstr_t t =  gram_codes;
	*result = m_pAgramtab->FindGrammems(t, grammems) ? TRUE : FALSE;	 
	return S_OK;
}



STDMETHODIMP CComGramTab::ProcessPOSAndGrammemsIfCan(BSTR in, BYTE *PartOfSpeech, QWORD *Grammems, BOOL* Result)
{
    _bstr_t g1 = in;
	*Result = m_pAgramtab->ProcessPOSAndGrammems ((const char*)g1, *PartOfSpeech, *Grammems) ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComGramTab::GetClauseTypeByName(BSTR bstrTypeName, long *lType)
{
	*lType = m_pAgramtab->GetClauseTypeByName((const char*)_bstr_t(bstrTypeName));
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
		*bstrName = _bstr_t(m_pAgramtab->GetClauseNameByType(type)).copy();	
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CComGramTab::GetGramCodeByGrammemsAndPartofSpeechIfCan(BYTE Pos, QWORD grammems, BSTR *gramcodes, BOOL* bRes)
{
	string str;
	*bRes = m_pAgramtab->GetGramCodeByGrammemsAndPartofSpeechIfCan(Pos, grammems, str);
	*gramcodes = _bstr_t(str.c_str()).copy();

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


