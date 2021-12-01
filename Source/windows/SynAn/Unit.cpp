// Unit.cpp : Implementation of CCOMUnit
#include "StdAfx.h"
#include "Synan_i.h"
#include "Unit.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMUnit

BOOL CCOMUnit::InitUnit(const CClause* pClause, const CMorphVariant&	SynVariant, int UnitNum, int iClauseNum)
{
	try
	{
		m_iClauseNum = iClauseNum;
		m_pClause = pClause;		
		if( !m_pClause )
			return FALSE;
		if( (UnitNum < 0) || ( UnitNum >= SynVariant.m_SynUnits.size() ) )		
			return FALSE;

		m_pUnit = &SynVariant.m_SynUnits[UnitNum];

	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}

STDMETHODIMP CCOMUnit::get_Type(int *pVal)
{
	try
	{
		*pVal = m_pUnit->m_Type;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMUnit::get_WordNum(long *pVal)
{
	try
	{
		if( m_pUnit->m_Type != EWord )
			return E_FAIL;

		*pVal = m_pUnit->m_SentPeriod.m_iFirstWord;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMUnit::get_ActiveHomonymNum(long *pVal)
{
	if( m_pUnit->m_Type != EWord )
		return E_FAIL;

	*pVal = m_pUnit->m_iHomonymNum;

	return S_OK;
}

STDMETHODIMP CCOMUnit::get_ActiveClauseTypeNum(long *pVal)
{
	if( m_pUnit->m_Type != EClause )
		return E_FAIL;

	*pVal = m_pUnit->m_iClauseTypeNum;

	return S_OK;
}

STDMETHODIMP CCOMUnit::get_ClauseNum(long *pVal)
{
	if( m_pUnit->m_Type != EClause )
		return E_FAIL;

	*pVal = m_iClauseNum;

	return S_OK;
}


STDMETHODIMP CCOMUnit::get_Grammems(hyper *pVal)
{
	try
	{

		if( !m_pUnit )
			return E_FAIL;

		*pVal = m_pUnit->m_iGrammems;

	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CCOMUnit::get_Gramcodes(BSTR *pVal)
{
	try
	{
		if( !m_pUnit )
			return E_FAIL;
		std::string s = m_pUnit->GetGramCodes();
		s = convert_to_utf8(s, this->m_pClause->GetOpt()->m_Language);
		*pVal = _bstr_t(s.c_str()).copy();

	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMUnit::get_GrammemsStr(BSTR *pVal)
{
	try
	{
		if( !m_pUnit )
			return E_FAIL;

		if (!m_pClause)
			return E_FAIL;


		std::string s = m_pUnit->GetGrammemsByAncodes();
		s = convert_to_utf8(s, this->m_pClause->GetOpt()->m_Language);

		if (m_pUnit->m_Type == EWord)
		{
			const CSynHomonym& H = m_pClause->GetWords()[m_pUnit->m_SentPeriod.m_iFirstWord].m_Homonyms[m_pUnit->m_iHomonymNum];
			if (H.m_bPassive)
				s = "PASSIVE " + s;
			if (m_pUnit->m_bReflexive)
				s = "REFLEXIVE " + s;
		};

		*pVal = _bstr_t(s.c_str()).copy();

	}
	catch(...)
	{
		return E_FAIL;
	}


	return S_OK;
}

STDMETHODIMP CCOMUnit::get_SimplePrepsCount(long *pVal)
{
	try
	{
		if( !m_pUnit )
			return E_FAIL;

		*pVal = m_pUnit->m_SimplePrepNos.size();

	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMUnit::get_SimplePrepId(long ii, long *pVal)
{
	try
	{
		if( !m_pUnit )
			return E_FAIL;

		if( (ii < 0) || (ii >= m_pUnit->m_SimplePrepNos.size()) )
			return E_FAIL;

		*pVal = m_pUnit->m_SimplePrepNos[ii];

	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMUnit::get_SimplePrepStr(long ii,BSTR *pVal)
{
	try
	{
		if( !m_pUnit )
			return E_FAIL;

		if( (ii < 0) || (ii >= m_pUnit->m_SimplePrepNos.size()) )
			return E_FAIL;
		
		if( !m_pClause->GetOpt() )
			return E_FAIL;

		std::string s = m_pClause->GetOpt()->GetOborDic()->m_Entries[m_pUnit->m_SimplePrepNos[ii]].m_OborotEntryStr;
		s = convert_to_utf8(s, this->m_pClause->GetOpt()->m_Language);

		*pVal = _bstr_t(s.c_str()).copy();

	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}
