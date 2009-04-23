// COMGroup.cpp : Implementation of CCOMGroup

#include "stdafx.h"
#include "comdef.h"
#include "SynAn.h"
#include "COMGroup.h"



STDMETHODIMP CCOMGroup::get_TypeStr(BSTR* pVal)
{
	try
	{
		string s = m_pOptions->GetGroupNameByIndex(m_pGroup->m_GroupType);
		*pVal = _bstr_t(s.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMGroup::get_FirstWord(long *pVal)
{
	try
	{
		*pVal = m_pGroup->m_iFirstWord;
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CCOMGroup::get_LastWord(long *pVal)
{
	try
	{
		*pVal = m_pGroup->m_iLastWord;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMGroup::get_MainGroupFirstWord(long *pVal)
{
	try
	{
		*pVal = m_pGroup->m_MainGroup.m_iFirstWord;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMGroup::get_MainGroupLastWord(long *pVal)
{
	try
	{
		*pVal = m_pGroup->m_MainGroup.m_iLastWord;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMGroup::get_Grammems(hyper *pVal)
{
	try
	{
		*pVal = m_pGroup->GetGrammems();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}



STDMETHODIMP CCOMGroup::get_MainWord(long *pVal)
{
	if(m_pGroup == NULL)
		return E_FAIL;

	*pVal = m_pGroup->m_MainWordNo;

	return S_OK;
}

