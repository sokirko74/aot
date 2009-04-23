// COMSentence.cpp : Implementation of CCOMSentence
#include "stdafx.h"
#include "SynAn.h"
#include "COMSentence.h"
#include "COMWord.h"
/////////////////////////////////////////////////////////////////////////////
// CCOMSentence



STDMETHODIMP CCOMSentence::get_PrimitiveClausesCount(long *pVal)
{
	try
	{
		*pVal = m_pSentence->m_vectorPrClauseNo.size();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMSentence::get_PrimitiveClause(long lClauseNum, IClause**pVal)
{
	try
	{
		if( (lClauseNum < 0) || (lClauseNum >= m_pSentence->m_vectorPrClauseNo.size()) )
			return E_FAIL;

		if( !m_pSentence )
			return E_FAIL;

		CComObject<CCOMClause>* pClause = new CComObject<CCOMClause>;

		pClause->Init(&m_pSentence->GetClause(m_pSentence->m_vectorPrClauseNo[lClauseNum]), m_pSentence);

		pClause->AddRef();
		*pVal = pClause;
		
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMSentence::get_WordsNum(long *pVal)
{
	try
	{
		*pVal = m_pSentence->m_Words.size();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMSentence::get_Word(long lWordNum, IWord **pVal)
{
	try
	{
		if( (lWordNum < 0) || (lWordNum >= m_pSentence->m_Words.size()) )
			return E_FAIL;

		CComObject<CCOMWord>* pWord = NULL;
		pWord = new CComObject<CCOMWord>;
		BOOL bRes = pWord->Init(&m_pSentence->m_Words[lWordNum], lWordNum, m_pSentence);
		if( !bRes )
		{
			delete pWord;
			return E_FAIL;
		}

		*pVal = pWord;
		pWord->AddRef();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}



STDMETHODIMP CCOMSentence::get_ClausesCount(long *pVal)
{
	*pVal = m_pSentence->GetClausesCount();
	return S_OK;
}

STDMETHODIMP CCOMSentence::get_Clause(long lClauseNum, IClause **piClause)
{
	if( (lClauseNum < 0) || (lClauseNum >= m_pSentence->GetClausesCount()  ) )
		return E_FAIL;

	CComObject<CCOMClause>* pClause = new CComObject<CCOMClause>;

	pClause->Init(&m_pSentence->GetClause(lClauseNum),  m_pSentence);

	pClause->AddRef();
	*piClause = pClause;
	
	return S_OK;
}


STDMETHODIMP CCOMSentence::CreateRelationsIterator(IRelationsIterator **piRelsIt)
{
	CComObject<CCOMRelationsIterator>* pRelIt = new CComObject<CCOMRelationsIterator>;
	pRelIt->Init(m_pSentence);
	pRelIt->AddRef();
	*piRelsIt = pRelIt;

	return S_OK;
}

STDMETHODIMP CCOMSentence::GetHomonymByClauseVar(long iWord, long iVar, long iClause, IHomonym **piHom)
{
	try
	{
		CSynHomonym H(m_pSentence);

		if (!m_pSentence->GetHomonymByClauseVariantInterface(iWord, iVar, iClause, H))
			return E_FAIL;

		CComObject<CCOMHomonym>* pHomonym = NULL;
		pHomonym = new CComObject<CCOMHomonym>;
		pHomonym->Init(H, m_pSentence);
		pHomonym->AddRef();
		*piHom = pHomonym;

	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}


STDMETHODIMP CCOMSentence::ClauseNoByPrimitiveClauseNo(long lPrClauseNo, long* lClauseNo)
{
	if( (lPrClauseNo < 0) || (lPrClauseNo >= m_pSentence->m_vectorPrClauseNo.size()) )
		return E_FAIL;

	if( !m_pSentence )
		return E_FAIL;

	*lClauseNo = m_pSentence->m_vectorPrClauseNo[lPrClauseNo];
	return S_OK;
}

STDMETHODIMP CCOMSentence::PrimitiveClauseNoByClauseNo(/*[in]*/long lClauseNo, /*[out, retval]*/long* lPrClauseNo)
{
	*lPrClauseNo = m_pSentence->GetPrimitiveClauseNoByClauseNo(lClauseNo);
	return S_OK;
}



STDMETHODIMP CCOMSentence::GetOborotStrByOborotId(long OborotId, BSTR *pVal)
{
	try
	{
		string Descr = m_pSentence->GetOpt()->m_pOborDic->m_Entries[OborotId].m_OborotEntryStr;
		*pVal = _bstr_t(Descr.c_str()).copy();
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;		
	}

	return S_OK;
}
