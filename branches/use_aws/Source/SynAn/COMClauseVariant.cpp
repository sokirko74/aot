// COMClauseVariant.cpp : Implementation of CCOMClauseVariant
#include "stdafx.h"
#include "SynAn.h"
#include "COMClauseVariant.h"
#include "COMGroup.h"
#include "COMClauseType.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMClauseVariant



BOOL CCOMClauseVariant::InitSynVariant(const CClause* pClause, CSVI pSynVar)
{
	if( !pClause )
		return FALSE;
	
	m_pClause = pClause;
	m_pSynVariant = pSynVar;
	const CMorphVariant& C = *pSynVar;

	if (pSynVar->m_ClauseTypeNo != -1)
		m_ClauseTypeOfThisSynVar = pClause->m_vectorTypes[pSynVar->m_ClauseTypeNo];
	return TRUE;
}

STDMETHODIMP CCOMClauseVariant::get_GroupsCount(long *pVal)
{
	try
	{
		*pVal = m_pSynVariant->m_vectorGroups.GetGroups().size();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMClauseVariant::get_Group(long lGroupNum, IGroup **pVal)
{	
	try
	{
		if(     (lGroupNum < 0) 
			|| (lGroupNum >= m_pSynVariant->m_vectorGroups.GetGroups().size()) 
		  )
			return E_FAIL;

		const CGroup& group =  m_pSynVariant->m_vectorGroups.GetGroups()[lGroupNum];

		CComObject<CCOMGroup>* pGroup = NULL;
		pGroup = new CComObject<CCOMGroup>;
		
		BOOL bRes = pGroup->Init(&group, GetOptions());
		if( !bRes )
			return FALSE;
		*pVal = pGroup;
		pGroup->AddRef();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMClauseVariant::get_UnitsCount(long *pVal)
{
	try
	{
		*pVal = m_pSynVariant->m_SynUnits.size();
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}



STDMETHODIMP CCOMClauseVariant::get_Subjects(long SubjectNo, long *pVal)
{
	if (SubjectNo >= m_pSynVariant->m_Subjects.size())
		return E_FAIL;

	*pVal = m_pSynVariant->m_Subjects[SubjectNo];

	return S_OK;
}

STDMETHODIMP CCOMClauseVariant::get_SubjectsCount(long *pVal)
{
	*pVal = m_pSynVariant->m_Subjects.size();
	return S_OK;
}

STDMETHODIMP CCOMClauseVariant::get_Predicate(long *pVal)
{
	*pVal = m_pSynVariant->m_iPredk;

	return S_OK;
}


STDMETHODIMP CCOMClauseVariant::get_Unit(long lUnitNum, ISyntaxUnit **pVal)
{
	try
	{
		CComObject<CCOMUnit>* pUnit;
		pUnit = new CComObject<CCOMUnit>;
		int iClauseNum = -1;

		if( m_pSynVariant->m_SynUnits[lUnitNum].m_Type == EClause )
		{
			iClauseNum = m_pClause->m_pSent->FindClauseIndexByPeriod(m_pSynVariant->m_SynUnits[lUnitNum].m_SentPeriod);
			if( iClauseNum == -1 )
			{
				delete pUnit;
				return E_FAIL;
			}
		}

		if( !pUnit->InitUnit(m_pClause, *(m_pSynVariant), lUnitNum, iClauseNum) )
			return E_FAIL;

		*pVal = pUnit;
		pUnit->AddRef();		
		
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMClauseVariant::get_ClauseRootNo(long *pVal)
{
	*pVal = m_pSynVariant->m_ClauseTypeNo;

	return S_OK;
}



int CCOMClauseVariant::GetUnitNumByWordNum(int iWordNum) const 
{
	return m_pSynVariant->UnitNoByWordNo(m_pClause->m_iFirstWord);
}


STDMETHODIMP CCOMClauseVariant::get_VariantWeight(int* pVal)
{
	*pVal = m_pSynVariant->m_iWeight;	

	return S_OK;
}
