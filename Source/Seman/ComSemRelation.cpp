// ComSemRelation.cpp : Implementation of CComSemRelation
#include "stdafx.h"
#include "Seman.h"
#include "ComSemRelation.h"
#include ".\comsemrelation.h"

/////////////////////////////////////////////////////////////////////////////
// CComSemRelation

CComSemRelation::CComSemRelation() 
{
	m_pRusStr = 0;
}


STDMETHODIMP CComSemRelation::get_SourceNodeNo(long *pVal)
{
	*pVal = m_pRelation->m_SourceNodeNo;

	return S_OK;
}

STDMETHODIMP CComSemRelation::get_TargetNodeNo(long *pVal)
{
	*pVal = m_pRelation->m_TargetNodeNo;
	return S_OK;
}

STDMETHODIMP CComSemRelation::get_Name(BSTR *pVal)
{
	*pVal = _bstr_t(m_pRelation->m_Valency.m_RelationStr.c_str()).copy();
	return S_OK;
}

STDMETHODIMP CComSemRelation::get_WordRealization(BSTR* pVal)
{
	string s = m_pRusStr->GetPrepOrConjFromSynReal(m_pRelation->m_SynReal);
	*pVal = _bstr_t(s.c_str()).copy();
	return S_OK;
}
