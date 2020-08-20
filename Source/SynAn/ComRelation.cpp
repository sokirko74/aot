#include "stdafx.h"
#include "Synan_i.h"
#include "comdef.h"
#include "ComRelation.h"


STDMETHODIMP CCOMRelation::get_SourceItemType(int* type)
{
	try
	{
		if( m_iSourceGroup != -1)
			*type = EGroup;
		else
			*type = EWord;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMRelation::get_TargetItemType(int* type)
{
	try
	{
		if( m_iTargetGroup != -1 )
			*type = EGroup;
		else
			*type = EWord;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMRelation::get_Name(BSTR *pVal)
{
	try
	{
		std::string s = GetOpt()->GetGroupNameByIndex(m_Relation.type);
		s = convert_to_utf8(s.c_str(), GetOpt()->m_Language);
		*pVal = _bstr_t(s.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CCOMRelation::get_SourceClauseNo(long *pVal)
{
	*pVal = m_Relation.m_SourceClause.m_iClauseNo;
	return S_OK;
}

STDMETHODIMP CCOMRelation::get_TargetClauseNo(long *pVal)
{
	*pVal = m_Relation.m_TargetClause.m_iClauseNo;
	return S_OK;
}

STDMETHODIMP CCOMRelation::get_SourceItemNo(long *pVal)
{
	if( m_iSourceGroup != -1 )
		*pVal = m_iSourceGroup;
	else
		*pVal = m_Relation.m_iFirstWord;
	return S_OK;
}

STDMETHODIMP CCOMRelation::get_TargetItemNo(long *pVal)
{
	if( m_iTargetGroup != -1 )
		*pVal = m_iTargetGroup;
	else
		*pVal = m_Relation.m_iLastWord;
	return S_OK;
}

