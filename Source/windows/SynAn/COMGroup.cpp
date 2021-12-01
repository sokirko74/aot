#include "StdAfx.h"
#include "comdef.h"
#include "COMGroup.h"
//#include "Synan_i.h"
#include "synan/SynCommonLib/SyntaxInit.h"
#include "synan/SynCommonLib/Group.h"


BOOL CCOMGroup::Init(const CGroup* pGroup, const CSyntaxOpt* pOptions)
{
	if (!pGroup)
		return FALSE;

	m_pGroup = pGroup;
	m_pOptions = pOptions;

	return TRUE;
}

const CSyntaxOpt* CCOMGroup::GetOpt() const
{
	return m_pOptions;
};




STDMETHODIMP CCOMGroup::get_TypeStr(BSTR* pVal)
{
	try
	{
		std::string s = m_pOptions->GetGroupNameByIndex(m_pGroup->m_GroupType);
		s  = convert_to_utf8(s.c_str(), GetOpt()->m_Language);
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

