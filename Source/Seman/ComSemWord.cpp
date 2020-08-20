// ComSemWord.cpp : Implementation of CComSemWord
#include "stdafx.h"
#include "seman_i.h"
#include "ComSemWord.h"

/////////////////////////////////////////////////////////////////////////////
// CComSemWord


STDMETHODIMP CComSemWord::get_WordStr(BSTR *pVal)
{	
	auto s = m_pWord->m_Word;
	s = convert_to_utf8(s, morphRussian);
	*pVal = _bstr_t(s.c_str()).copy();
	return S_OK;
}

STDMETHODIMP CComSemWord::get_Lemma(BSTR *pVal)
{
	auto s = m_pWord->m_Lemma;
	s = convert_to_utf8(s, morphRussian);
	*pVal = _bstr_t(s.c_str()).copy();

	return S_OK;
}

STDMETHODIMP CComSemWord::get_SyntaxWordNo(long *pVal)
{
	*pVal = m_pWord->m_WordNo;

	return S_OK;
}

STDMETHODIMP CComSemWord::get_Poses(long *pVal)
{
	*pVal = m_pWord->m_Poses;
	return S_OK;
}

STDMETHODIMP CComSemWord::get_Grammems(hyper *pVal)
{
	*pVal = m_pWord->GetAllGrammems();

	return S_OK;
}
