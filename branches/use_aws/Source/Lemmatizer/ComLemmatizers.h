//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//========== Sokirko Alexey sokirko@online.ru
//================================

#ifndef __COMLEMMATIZERS_H_
#define __COMLEMMATIZERS_H_

#include "../LemmatizerLib/Lemmatizers.h"
#include "ComParadigm.h"

#pragma warning (disable : 4250)

#include "lemmatizer_resource.h"       // main symbols
#include "ParadigmCollection.h"


extern bool CheckPersonalLicense();


class ATL_NO_VTABLE CCOMLemmatizer : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ILemmatizer, &IID_ILemmatizer, &LIBID_LEMMATIZERLib>
	
{
	
public:
	CLemmatizer* m_pLemmatizer;
	

BEGIN_COM_MAP(CCOMLemmatizer)
	COM_INTERFACE_ENTRY(ILemmatizer)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	STDMETHODIMP LoadDictionariesRegistry();
	STDMETHODIMP put_MaximalPrediction(/*[in]*/ BOOL newVal);
	STDMETHODIMP get_UseStatistic(/*[out, retval]*/ BOOL *pVal);
	STDMETHODIMP put_UseStatistic(/*[in]*/ BOOL newVal);

	/*
		function CreateParadigmCollectionInner is not included into idl
	*/
		
	STDMETHOD(CreateParadigmCollectionInner)(bool bNorm, /*[in]*/ BSTR form, /*[in]*/ BOOL capital,BOOL use_prediction, /*[out, retval]*/ IParadigmCollection* *pVal);
	STDMETHOD(CreateParadigmCollectionFromNorm)(/*[in]*/ BSTR norm, /*[in]*/ BOOL capital, BOOL use_prediction,/*[out, retval]*/ IParadigmCollection* *pVal);
	STDMETHOD(CreateParadigmCollectionFromForm)(/*[in]*/ BSTR form, /*[in]*/ BOOL capital, BOOL use_prediction,/*[out, retval]*/ IParadigmCollection* *pVal);
	STDMETHOD(CheckABC)(BSTR Word, BOOL* Result) ;
	STDMETHOD(CreateParadigmFromID)(/*[in]*/ long id, /*[out, retval]*/ IParadigm* *pVal);
	STDMETHOD(LoadStatisticRegistry)(/*[in]*/ idlSubjectEnum subj);

};


class CCOMLemmatizerRussian : 
	public CComCoClass<CCOMLemmatizerRussian, &CLSID_LemmatizerRussian>,
	public CCOMLemmatizer,
	public CLemmatizerRussian
{
public:
	CCOMLemmatizerRussian() : CLemmatizerRussian()
	{ 
		m_pLemmatizer = this;
	}
	
	DECLARE_REGISTRY_RESOURCEID(IDR_COMLEMMATIZERRUSSIAN)
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	
};

class  CCOMLemmatizerEnglish : 
	public CComCoClass<CCOMLemmatizerEnglish, &CLSID_LemmatizerEnglish>,
	public CCOMLemmatizer,
	public CLemmatizerEnglish
{
public:
	CCOMLemmatizerEnglish() 
	{ 
		m_pLemmatizer = this;
	}
	
	DECLARE_REGISTRY_RESOURCEID(IDR_COMLEMMATIZERENGLISH)
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	
};


class  CCOMLemmatizerGerman : 
	public CComCoClass<CCOMLemmatizerGerman, &CLSID_LemmatizerGerman>,
	public CCOMLemmatizer,
	public CLemmatizerGerman
{
public:
	CCOMLemmatizerGerman() 
	{ 
		m_pLemmatizer = this;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_COMLEMMATIZERGERMAN)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	
};



#endif //__LEMMATIZERRUS_H_
