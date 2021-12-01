
#include "StdAfx.h"
#include "../Graphan/ComGraphmatFile.h"
#include "morphan/LemmatizerLib/GraphanAndMorphanHolder.h"
#include "Lemmatizer_i.h"
#include "ComPlmLineCollection.h"
#include <sstream>
#include <fstream>

STDMETHODIMP CCOMPLMLineCollection::get_Count(/*[out, retval]*/ long* pVal)
{
	*pVal = m_PlmItems.size();
	return S_OK;
}

STDMETHODIMP CCOMPLMLineCollection::get_Item(/*[in]*/ long pos, /*[out, retval]*/ BSTR* pVal)
{
	if (pos >= m_PlmItems.size())
		return E_FAIL;
	std::string s = m_PlmItems[pos];
	s = convert_to_utf8(s, m_pLemmatizer->m_Language);
	*pVal = _bstr_t(s.c_str()).copy();
	return S_OK;
}
STDMETHODIMP CCOMPLMLineCollection::put_Item(/*[in]*/ long pos, /*[in]*/ BSTR newVal)
{
	if (pos >= m_PlmItems.size())
		return E_FAIL;
	std::string s = (const char*)_bstr_t(newVal);
	s = convert_from_utf8(s.c_str(), m_pLemmatizer->m_Language);
	m_PlmItems[pos] = s;
	return S_OK;
}
STDMETHODIMP CCOMPLMLineCollection::AddLine(BSTR LineStr)
{
	std::string s = (const char*)_bstr_t(LineStr);
	s = convert_from_utf8(s.c_str(), m_pLemmatizer->m_Language);
	m_PlmItems.push_back(s);
	return S_OK;
}

STDMETHODIMP CCOMPLMLineCollection::Remove(/*[in]*/ long pos)
{
	if (pos >= m_PlmItems.size())
		return E_FAIL;
	m_PlmItems.erase(m_PlmItems.begin() + pos);
	return S_OK;
}

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
	m_PlmItems.clear();
	return S_OK;
}

STDMETHODIMP CCOMPLMLineCollection::ProcessHyphenWords(IUnknown* piGraphan)
{
	CComGraphmatFile* pGer = reinterpret_cast<CComGraphmatFile*>(piGraphan);
	if (!::ProcessHyphenWords(m_pLemmatizer, pGer))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CCOMPLMLineCollection::CopyItems(IUnknown* piPlmLinesFrom)
{
	CCOMPLMLineCollection* pFrom = reinterpret_cast<CCOMPLMLineCollection*>(piPlmLinesFrom);
	m_PlmItems.clear();
	m_PlmItems = pFrom->m_PlmItems;
	return S_OK;
}

STDMETHODIMP CCOMPLMLineCollection::get_Lemmatizer(IUnknown** pVal)
{
	CComObject<CCOMLemmatizer>* lemmatizer = new CComObject<CCOMLemmatizer>;
	if (!lemmatizer) return E_FAIL;
	lemmatizer->AddRef();
	lemmatizer->m_pLemmatizer = m_pLemmatizer;
	*pVal = lemmatizer;
	return S_OK;
}
