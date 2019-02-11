// RelationsIterator.cpp : Implementation of CCOMRelationsIterator
#include "stdafx.h"
#include "SynAn.h"
#include "ComRelationIterator.h"


STDMETHODIMP CCOMRelationsIterator::AddClauseNoAndVariantNo(long iClauseNo, long iVarNo)
{
	CRelationsIterator::AddClauseNoAndVariantNo(iClauseNo, iVarNo);
	return S_OK;
}

STDMETHODIMP CCOMRelationsIterator::get_RelationsCount(long *pVal)
{
	*pVal = m_vectorIRelations.size();
	return S_OK;
}

STDMETHODIMP CCOMRelationsIterator::BuildRelations()
{
	if (!CRelationsIterator::BuildRelations())
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP CCOMRelationsIterator::get_Relation(long iRel, IRelation **pVal)
{
	try
	{
		if( (iRel < 0) || (iRel >= m_vectorIRelations.size()) )
			return E_FAIL;
		CCOMRelation* p  = new CComObject<CCOMRelation>;
		p->CopyFrom( m_vectorIRelations[iRel] );
		p->AddRef();
		
		*pVal = p;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMRelationsIterator::get_FirmGroupsCount(long *pVal)
{
	*pVal = m_vectorGroups.size();
	return S_OK;
}

STDMETHODIMP CCOMRelationsIterator::get_FirmGroup(long lGroupNum, IGroup **pVal)
{
	try
	{
		if( (lGroupNum < 0) || (lGroupNum >= m_vectorGroups.size()) )
			return E_FAIL;

		CComObject<CCOMGroup>* pGroup = NULL;
		pGroup = new CComObject<CCOMGroup>;
		BOOL bRes = pGroup->Init(&m_vectorGroups[lGroupNum], m_pSent->m_pSyntaxOptions);
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


STDMETHODIMP CCOMRelationsIterator::Reset()
{
	try
	{
		ResetAll();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

