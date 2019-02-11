// ComSemWord.cpp : Implementation of CComSemWord
#include "stdafx.h"
#include "Seman.h"
#include "ComSemWord.h"

/////////////////////////////////////////////////////////////////////////////
// CComSemWord


STDMETHODIMP CComSemWord::get_WordStr(BSTR *pVal)
{	
	*pVal = _bstr_t(m_pWord->m_Word.c_str()).copy();


	return S_OK;
}

STDMETHODIMP CComSemWord::get_Lemma(BSTR *pVal)
{
	*pVal = _bstr_t(m_pWord->m_Lemma.c_str()).copy();

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
