// COMSentencesCollection.cpp : Implementation of CCOMSentencesCollection

#include "stdafx.h"
#include "SynAn.h"
#include "COMSentencesCollection.h"
#include "../Lemmatizer/ComPlmLineCollection.h"
//#include "../RmlThes/ComThesaurus.h"
#include "../StructDict/ComDictionary.h"
#import "../../bin/Lemmatizer.tlb"



/////////////////////////////////////////////////////////////////////////////

/*STDMETHODIMP CCOMSentencesCollection::SetThesaurus(UINT ThesId, IUnknown *Thes)
{
	CComThesaurus*p = reinterpret_cast<CComThesaurus*>(Thes);
	CSentencesCollection::SetThesaurus (ThesId, p);
	return S_OK;
}
*/
STDMETHODIMP CCOMSentencesCollection::SetLemmatizer(IUnknown *Interf)
{
	CSentencesCollection::SetLemmatizer(  reinterpret_cast<CCOMLemmatizerRussian*>(Interf) );
	return S_OK;
}

STDMETHODIMP CCOMSentencesCollection::SetOborDic(IUnknown *Interf)
{
	CSentencesCollection::SetOborDic(  reinterpret_cast<CComDictionary*>(Interf) );
	return S_OK;
}


STDMETHODIMP CCOMSentencesCollection::InitializeProcesser()
{
	if (!m_pSyntaxOptions) return E_FAIL;

	if (!CSentencesCollection::InitializeProcesser())
			return E_FAIL;		
	return S_OK;
}

STDMETHODIMP CCOMSentencesCollection::ProcessData(IUnknown *piPlmLine)
{
	try
	{
		LEMMATIZERLib::IPLMLineCollectionPtr piPLMLinePtr;

		piPLMLinePtr.Attach(reinterpret_cast<LEMMATIZERLib::IPLMLineCollection*>(piPlmLine));

		CCOMPLMLineCollection* pPlm = reinterpret_cast<CCOMPLMLineCollection*>(piPlmLine);

		if (!CSentencesCollection::ProcessData(pPlm)) return E_FAIL;

		piPLMLinePtr.Detach();
	
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}



STDMETHODIMP CCOMSentencesCollection::get_SentencesCount(long *pVal)
{
	*pVal = m_vectorSents.size();

	return S_OK;
}


STDMETHODIMP CCOMSentencesCollection::get_Sentence(long i, ISentence **pVal)
{
	try
	{
		if( (i < 0) || (i >= m_vectorSents.size()) )
			return E_FAIL;

		CComObject<CCOMSentence>* pSent;
		pSent = new CComObject<CCOMSentence>;
		pSent->AddRef();
		pSent->m_pSentence = m_vectorSents[i];
		*pVal = pSent;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}




STDMETHODIMP CCOMSentencesCollection::ClearSentences()
{
	try
	{
		CSentencesCollection::ClearSentences();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}


STDMETHODIMP CCOMSentencesCollection::put_EnableThesauri(BOOL newVal)
{
	if (!m_pSyntaxOptions) return E_FAIL;
	SetEnableAllThesauri(newVal ? true : false);
	return S_OK;
}


STDMETHODIMP CCOMSentencesCollection::get_EnableThesauri(BOOL* Val)
{
	if (!m_pSyntaxOptions) return E_FAIL;
	*Val =  m_pSyntaxOptions->m_bEnableLocThesaurus ? TRUE : FALSE;
	return S_OK;
}




STDMETHODIMP CCOMSentencesCollection::put_SilentMode(BOOL newVal)
{
	try
	{
		CSentencesCollection::put_SilentMode(newVal ? true : false);
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}





STDMETHODIMP CCOMSentencesCollection::get_KillHomonymsMode(UINT *pVal)
{
	if (!m_pSyntaxOptions) return E_FAIL;
	*pVal = m_pSyntaxOptions->m_KillHomonymsMode ? true : false;
	return S_OK;
}

STDMETHODIMP CCOMSentencesCollection::put_KillHomonymsMode(UINT newVal)
{
	if (!m_pSyntaxOptions) return E_FAIL;
	m_pSyntaxOptions->m_KillHomonymsMode = (KillHomonymsEnum)newVal;
	return S_OK;
}


STDMETHODIMP CCOMSentencesCollection::get_DeleteEqualPrimitiveClauseVariants(BOOL *pVal)
{
	*pVal =   m_bDeleteEqualPrimitiveClauseVariants  ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CCOMSentencesCollection::put_DeleteEqualPrimitiveClauseVariants(BOOL newVal)
{
	m_bDeleteEqualPrimitiveClauseVariants = newVal  ? true : false;
	return S_OK;
}

STDMETHODIMP CCOMSentencesCollection::get_SyntaxLanguage(int* pVal)
{
	if (!m_pSyntaxOptions) return E_FAIL;
	*pVal = m_pSyntaxOptions->m_Language;
	return S_OK;
}

STDMETHODIMP CCOMSentencesCollection::put_SyntaxLanguage(int newVal)
{
	CreateOptions( (MorphLanguageEnum)newVal);
	if (!m_pSyntaxOptions) return E_FAIL;
	return S_OK;
}



STDMETHODIMP CCOMSentencesCollection::get_EnableProgressBar(BOOL* pVal)
{
	*pVal = (m_bEnableProgressBar) ? TRUE : FALSE;

	return S_OK;
}

STDMETHODIMP CCOMSentencesCollection::put_EnableProgressBar(BOOL newVal)
{
	m_bEnableProgressBar = (newVal == TRUE) ? true : false;

	return S_OK;
}


