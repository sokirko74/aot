//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//========== Sokirko Alexey sokirko@yandex.ru
//================================
#pragma warning(disable:4786)
#pragma warning(disable:4530)

#include "stdafx.h"
#include "Lemmatizer.h"
#include "ComParadigm.h"
STDMETHODIMP  CCOMParadigm::get_ParadigmID(/*[out, retval]*/ DWORD *pVal)
{
	*pVal = GetParadigmId();;
	return S_OK;
}


STDMETHODIMP  CCOMParadigm::put_ParadigmID(/*[in]*/ DWORD newVal)
{
	if ( !SetParadigmId(newVal) )
		return (E_FAIL);

	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_Count(/*[out, retval]*/ DWORD *pVal)
{
	*pVal = GetCount();;
	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_Ancode(/*[in]*/ DWORD pos, /*[out, retval]*/ BSTR *pVal)
{
	*pVal = _bstr_t(GetAncode(pos).c_str()).copy();
	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_Norm(/*[out, retval]*/ BSTR *pVal)
{
	*pVal = _bstr_t(GetWordForm(0).c_str()).copy();
	return S_OK;

}


STDMETHODIMP  CCOMParadigm::get_Form(/*[in]*/ DWORD pos, /*[out, retval]*/ BSTR *pVal)
{
	*pVal = _bstr_t( GetWordForm(pos).c_str() ).copy();
	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_TypeAncode(BSTR *pVal)
{
	*pVal = _bstr_t(GetCommonAncode().c_str()).copy();
	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_Founded(/*[out, retval]*/ BOOL *pVal)
{
	*pVal = m_bFound;
	return S_OK;
}

//gri
STDMETHODIMP  CCOMParadigm::get_SrcAncode(/*[out, retval]*/ BSTR *pVal)
{
	*pVal = _bstr_t(GetSrcAncode().c_str()).copy() ;
	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_SrcNorm(/*[out, retval]*/ BSTR *pVal)
{
	*pVal = _bstr_t(GetSrcNorm().c_str()).copy() ;	
	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_HomonymWeightWithForm(/*[in]*/ DWORD pos, /*[out, retval]*/ int *pVal)
{
	*pVal = GetHomonymWeightWithForm(pos);
	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_HomonymWeight(/*[out, retval]*/ int *pVal)
{
	*pVal = GetHomonymWeight();
	return S_OK;
}

STDMETHODIMP  CCOMParadigm::get_WordWeight(/*[out, retval]*/ int *pVal)
{
	*pVal = GetWordWeight();
	return S_OK;
}


STDMETHODIMP CCOMParadigm::get_Accent(DWORD pos, BYTE* pVal)
{
	*pVal = GetAccentedVowel(pos);
	return S_OK;
}

STDMETHODIMP CCOMParadigm::get_BaseLength(UINT* pVal)
{
	*pVal = m_InputWordBase.length();
	
	return S_OK;
}

STDMETHODIMP CCOMParadigm::get_LemmaPrefixLength(int* pVal)
{
	
	*pVal = GetLemmaPrefixLength();
	return S_OK;
}
