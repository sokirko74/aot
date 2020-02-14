// PLMLineCollection.cpp : Implementation of CCOMPLMLineCollection
#include "StdAfx.h"

#include "Lemmatizer.h"
#include "ComPlmLineCollection.h"
#include <sstream>
#include <fstream>



STDMETHODIMP CCOMPLMLineCollection::ProcessPlmLines(/*[in]*/ IUnknown* piGraphmatFile)
{
	CComGraphmatFile* pGer = reinterpret_cast<CComGraphmatFile*>(piGraphmatFile);
	if (!CPlmLineCollection::ProcessPlmLines(pGer))
		return E_FAIL;
	return S_OK;
}


STDMETHODIMP CCOMPLMLineCollection::SaveToFile(/*[in]*/ BSTR filename)
{
	if (!CPlmLineCollection::SaveToFile((const char*)_bstr_t(filename)) )
		return E_FAIL;
	return S_OK;
}


STDMETHODIMP CCOMPLMLineCollection::AttachLemmatizer(/*[in]*/ ILemmatizer* pVal)
{
	CCOMLemmatizer* piLemmatizer = reinterpret_cast<CCOMLemmatizer*>(pVal);
	if (piLemmatizer != NULL)
		piLemmatizer->AddRef();
	m_pComLemmatizer = 0;
	m_pComLemmatizer = piLemmatizer;
	m_pLemmatizer = piLemmatizer->m_pLemmatizer;
	return S_OK;
}



STDMETHODIMP CCOMPLMLineCollection::Clear()
{
	m_Items.clear();
	return S_OK;
}

STDMETHODIMP CCOMPLMLineCollection::ProcessHyphenWords(IUnknown* piGraphan)
{
	CComGraphmatFile* pGer = reinterpret_cast<CComGraphmatFile*>(piGraphan);
	if (!m_pLemmatizer->ProcessHyphenWords(pGer))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CCOMPLMLineCollection::CopyItems(IUnknown* piPlmLinesFrom)
{
	CCOMPLMLineCollection* pFrom = reinterpret_cast<CCOMPLMLineCollection*>(piPlmLinesFrom);
	m_Items.clear();
	m_Items = pFrom->m_Items;
	return S_OK;
}
