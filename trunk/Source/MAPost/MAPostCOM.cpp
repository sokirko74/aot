// MAPostCOM.cpp : Implementation of CCOMMAPost
#include "stdafx.h"
#include "..\Agramtab\ComGramTab.h"
#include "MAPost.h"
#include "MAPostCOM.h"
#include "..\common\utilit.h"
#include "..\common\rus_numerals.h"
#include "..\Lemmatizer\ComPlmLineCollection.h"
#import "..\..\Bin\Lemmatizer.tlb"

// language specific 
extern CPostMorphInteface* NewGermanPostMorph(const CLemmatizer*, const CAgramtab*);
extern CPostMorphInteface* NewRussianPostMorph(const CLemmatizer*, const CAgramtab*);



CCOMMAPost::CCOMMAPost()
{
	m_pPostMorphInterface = 0;
};

CCOMMAPost::~CCOMMAPost()
{
	if (m_pPostMorphInterface) delete m_pPostMorphInterface;
};

STDMETHODIMP CCOMMAPost::ProcessData(IUnknown *piInputTextItems, IUnknown **piOutputTextItems)
{
	const CCOMPLMLineCollection* In = reinterpret_cast<const CCOMPLMLineCollection*>(piInputTextItems);

	LEMMATIZERLib::IPLMLineCollectionPtr piPlmLines;
	HRESULT hr =  piPlmLines.CreateInstance(__uuidof(LEMMATIZERLib::PLMLineCollection));

	CCOMPLMLineCollection* Out = reinterpret_cast<CCOMPLMLineCollection*>(piPlmLines.GetInterfacePtr());
	Out->AddRef();

	if (!m_pPostMorphInterface->ProcessData (In, *Out))
			return E_FAIL;

	*piOutputTextItems = Out;

	return S_OK;
}



STDMETHODIMP CCOMMAPost::Init(int Language, IUnknown* Lemmatizer, IUnknown* GramTab)
{
	assert (!m_pPostMorphInterface);
	
	if (Language  == morphRussian)
		 m_pPostMorphInterface = NewRussianPostMorph (reinterpret_cast<CCOMLemmatizerRussian*>(Lemmatizer),
							reinterpret_cast<CComGramTab*>(GramTab)->GetGramTab());
	else
		if (Language  == morphGerman)
			m_pPostMorphInterface = NewGermanPostMorph (reinterpret_cast<CCOMLemmatizerGerman*>(Lemmatizer),
							reinterpret_cast<CComGramTab*>(GramTab)->GetGramTab());
		else
			return E_FAIL;
	

	if (!m_pPostMorphInterface)
		return E_FAIL;
			
	return S_OK;
}

STDMETHODIMP CCOMMAPost::get_bCanChangeInputText(BOOL *pVal)
{
	assert (m_pPostMorphInterface);
	*pVal =  m_pPostMorphInterface->m_bCanChangeInputText? TRUE: FALSE; 
	return S_OK;
};

STDMETHODIMP CCOMMAPost::put_bCanChangeInputText(BOOL pVal)
{

	m_pPostMorphInterface->m_bCanChangeInputText = (pVal == TRUE); 
	return S_OK;
};


STDMETHODIMP CCOMMAPost::get_bHumanFriendlyOutput(BOOL *pVal)
{
	assert (m_pPostMorphInterface);
	*pVal =  m_pPostMorphInterface->m_bHumanFriendlyOutput? TRUE: FALSE; 
	return S_OK;
};

STDMETHODIMP CCOMMAPost::put_bHumanFriendlyOutput(BOOL pVal)
{
	m_pPostMorphInterface->m_bHumanFriendlyOutput = (pVal == TRUE); 
	return S_OK;
};
