// ComSemNode.cpp : Implementation of CComSemNode
#include "stdafx.h"
#include "Seman.h"
#include "ComSemNode.h"
#include "ComSemWord.h"
#include ".\comsemnode.h"

/////////////////////////////////////////////////////////////////////////////
// CComSemNode

STDMETHODIMP CComSemNode::get_DictType(idlDictTypeEnum *pVal)
{
	// TODO: Add your implementation code here

	*pVal = InternalDictTypeToIdl (m_pNode->GetType()); 
	return S_OK;
}



STDMETHODIMP CComSemNode::get_UnitNo(WORD *pVal)
{
	*pVal = m_pNode->GetUnitNo();
	return S_OK;
}


STDMETHODIMP CComSemNode::get_WordsCount(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_pNode->GetWordsSize();

	return S_OK;
}

STDMETHODIMP CComSemNode::get_Poses(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_pNode->GetNodePoses();

	return S_OK;
}


STDMETHODIMP CComSemNode::get_Words(long pos, IComSemWord **pVal)
{
	CComObject<CComSemWord>* p = new  CComObject<CComSemWord>;
	p->AddRef();
	p->m_pWord = &(m_pNode->GetWord(pos));
	*pVal = p;
	return S_OK;
}

STDMETHODIMP CComSemNode::get_GramCodes(BSTR *pVal)
{
	*pVal = _bstr_t(m_pNode->m_GramCodes.c_str()).copy();	
	return S_OK;
}


STDMETHODIMP CComSemNode::get_Grammems(hyper *pVal)
{
	*pVal = m_pNode->GetGrammems();

	return S_OK;
}

STDMETHODIMP CComSemNode::get_ClauseNo(long *pVal)
{
	*pVal = m_pNode->m_ClauseNo;

	return S_OK;
}





STDMETHODIMP CComSemNode::get_RelOperatorsCount(int* pVal)
{
	*pVal = m_pNode->m_RelOperators.size();

	return S_OK;
}

STDMETHODIMP CComSemNode::get_RelOperators(int Index, BSTR* pVal)
{
	if (			( Index >= m_pNode->m_RelOperators.size())
			||		( Index < 0)
		)
		return E_FAIL;
	*pVal = _bstr_t(m_pNode->m_RelOperators[Index].c_str()).copy();	

	return S_OK;
}

STDMETHODIMP CComSemNode::get_Prep(BSTR* pVal)
{
	
	const CSynRealization& R = m_pNode->m_SynReal;
	string s = m_pRusStr->GetPrepOrConjFromSynReal(R);
	*pVal = _bstr_t(s.c_str()).copy();
	

	return S_OK;
}
