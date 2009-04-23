//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//========== Sokirko Alexey sokirko@yandex.ru
//================================
#include "stdafx.h"
#include "ComLemmatizers.h"
#include "ComParadigm.h"
#include "Lemmatizer.h"
#include "../LemmatizerLib/Lemmatizers.h"



extern bool CheckPersonalLicense();
STDMETHODIMP CCOMLemmatizer::LoadDictionariesRegistry() 
{	

	if (!CheckPersonalLicense())
		if (!CheckEvaluationTime())
			return E_FAIL;
	string strError;
	m_pLemmatizer->LoadDictionariesRegistry(strError);
	if (	m_pLemmatizer->m_bLoaded )
	{
		return S_OK;
	}
	else
	{
		//ErrorMessage ("This is a special version for Andrey Soloninkin <sol@nd.ru>, who failed to use this library on WinMe"); 
		return E_FAIL;
	}
};



STDMETHODIMP CCOMLemmatizer::put_MaximalPrediction(/*[in]*/ BOOL newVal)
{
	m_pLemmatizer->m_bMaximalPrediction = (newVal == TRUE);
	return S_OK;
}

STDMETHODIMP CCOMLemmatizer::get_UseStatistic(/*[out, retval]*/ BOOL *pVal)
{
	*pVal = m_pLemmatizer->m_bUseStatistic;
	return S_OK;
}

STDMETHODIMP CCOMLemmatizer::put_UseStatistic(/*[in]*/ BOOL newVal)
{
	m_pLemmatizer->m_bUseStatistic = (newVal == TRUE);
	return S_OK;
}



/*
	function CreateParadigmCollectionInner is not included into idl
*/
	
STDMETHODIMP CCOMLemmatizer::CreateParadigmCollectionInner(bool bNorm, /*[in]*/ BSTR form, /*[in]*/ BOOL capital, BOOL use_prediction, /*[out, retval]*/ IParadigmCollection* *pVal)
{
	string WordStr = (const char*) _bstr_t (form);
	vector<CFormInfo> Vec;
    m_pLemmatizer->CreateParadigmCollection(bNorm, WordStr, (capital?true:false), (use_prediction?true:false), Vec);
	CComObject<CCOMParadigmCollection>* res_collection = NULL;
	res_collection = new CComObject<CCOMParadigmCollection>; 
	if  (!res_collection) return E_FAIL;
	res_collection->AddRef();
	*pVal = res_collection;
	for (long i=0; i < Vec.size(); i++)
	{
		CComObject<CCOMParadigm>* res_paradigm = new CComObject<CCOMParadigm>;
		if  (!res_paradigm) return E_FAIL;
		res_paradigm->Copy(Vec[i]);
		res_paradigm->AddRef();
		res_collection->push_back(res_paradigm);
		res_paradigm = NULL;
	};
	return S_OK;
	
};

STDMETHODIMP CCOMLemmatizer::CreateParadigmCollectionFromNorm(/*[in]*/ BSTR norm, /*[in]*/ BOOL capital, BOOL use_prediction, /*[out, retval]*/ IParadigmCollection* *pVal)
{
	return CreateParadigmCollectionInner(true,  norm, capital, use_prediction, pVal);
};
STDMETHODIMP CCOMLemmatizer::CreateParadigmCollectionFromForm(/*[in]*/ BSTR form, /*[in]*/ BOOL capital, BOOL use_prediction,/*[out, retval]*/ IParadigmCollection* *pVal)
{
	return CreateParadigmCollectionInner(false,  form, capital, use_prediction, pVal);	
};


STDMETHODIMP CCOMLemmatizer::CheckABC(BSTR Word, BOOL* Result) 
{
	string s = (const char*)_bstr_t(Word);
	RmlMakeUpper(s,m_pLemmatizer->GetLanguage());
	*Result =  m_pLemmatizer->CheckABC(s) ?  TRUE : FALSE;
	return S_OK;
};

STDMETHODIMP CCOMLemmatizer::CreateParadigmFromID(/*[in]*/ long id, /*[out, retval]*/ IParadigm* *pVal)
{
	CComObject<CCOMParadigm>* result = NULL;
	try
	{
		CFormInfo P;
		if (!m_pLemmatizer->CreateParadigmFromID(id, P))
			return E_FAIL;
		result = new CComObject<CCOMParadigm>;
		if (result == NULL)
			return E_FAIL;
		result->Copy(P);
		result->AddRef();
		*pVal = result;
	}
	catch(...)
	{
		if (result)
			delete result;
		return E_FAIL;
	}
	return S_OK;
}
STDMETHODIMP CCOMLemmatizer::LoadStatisticRegistry(/*[in]*/ idlSubjectEnum subj)
{
	return m_pLemmatizer->LoadStatisticRegistry((SubjectEnum)subj) ? S_OK : E_FAIL; 
};


