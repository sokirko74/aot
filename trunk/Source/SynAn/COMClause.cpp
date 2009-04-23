// COMClause.cpp : Implementation of CCOMClause

#include "stdafx.h"
#include "COMClauseType.h"
#include "COMGroup.h"
#include "COMClause.h"


STDMETHODIMP CCOMClause::get_FirstWord(long *pVal)
{
	try
	{
		*pVal = m_pClause->m_iFirstWord;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}


STDMETHODIMP CCOMClause::get_LastWord(long *pVal)
{
	try
	{
		*pVal = m_pClause->m_iLastWord;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMClause::get_AntecedentWord(long *pVal)
{
	try
	{
		*pVal = m_pClause->m_AntecedentWordNo;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMClause::get_RelativeWord(long *pVal)
{
	try
	{
		*pVal = m_pClause->m_RelativeWord.m_WordNo;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMClause::get_VariantsCount(long *pVal)
{
	try
	{
		*pVal = m_pClause->GetSynVariantsCount();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMClause::get_ClauseVariant(long lVarNum, IClauseVariant **pVal)
{
	CComObject<CCOMClauseVariant>* pClauseVar = NULL;
	try
	{

		CSVI pSynVar = m_pClause->GetSynVariantByNo(lVarNum);

		if( !m_pClause->IsValidCSVI(pSynVar) )
				return E_FAIL;

		pClauseVar = new CComObject<CCOMClauseVariant>;
		BOOL bRes = pClauseVar->InitSynVariant( m_pClause, pSynVar);
		if( !bRes )
		{	
			delete pClauseVar;
			return E_FAIL;
		}

		*pVal = pClauseVar;
		pClauseVar->AddRef();
	}
	catch(...)
	{
		if( pClauseVar )
			delete pClauseVar;
		return E_FAIL;
	}
	return S_OK;
}


STDMETHODIMP CCOMClause::get_ClauseRootsCount(long *pVal)
{
	try
	{		
		*pVal = m_pClause->m_vectorTypes.size();

	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMClause::get_ClauseRoots(long iNum, IClauseRoot **pVal)
{
	try
	{
		if( (iNum < 0) || (iNum >= m_pClause->m_vectorTypes.size()) )
			return E_FAIL;
			

		CComObject<CCOMClauseType>* pClauseType;
		pClauseType = new CComObject<CCOMClauseType>;

		if( !pClauseType->Init(&m_pClause->m_vectorTypes[iNum], GetOpt() ) )
			return E_FAIL;

		*pVal = pClauseType;
		pClauseType->AddRef();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}


STDMETHODIMP CCOMClause::get_Description(BSTR *pVal)
{
	try
	{

		string S = " ";
		for(int i = 0 ; i < m_pClause->m_vectorConjs.size(); i++)		
		{
			int iConjIndex = m_pClause->m_vectorConjs[i].m_index;

			switch(m_pClause->m_vectorConjs[i].m_FromWhere)
			{
			case FROM_OBOR_DIC:
				{
					S += "\"";
					S += GetOpt()->m_pOborDic->m_Entries[iConjIndex].m_OborotEntryStr;
					S += "\"";
					break;
				}
			case FROM_SUB_CONJ:
				{
					S += GetOpt()->m_pOborDic->GetSubConjs()[iConjIndex];
					break;
				}
			
			}
			S += " ";
		}	
		if (m_pClause->IsRelative())
			S = "Relative " + S;
		*pVal = _bstr_t(S.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}




STDMETHODIMP CCOMClause::get_ConjsCount(long *pVal)
{
	try
	{
		*pVal = m_pClause->m_vectorConjs.size();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}



STDMETHODIMP CCOMClause::get_ConjStr(int iNum, BSTR *pVal)
{
	try
	{
		char strBuf[256];
		strcpy(strBuf," ");
		if( (iNum < 0) || (iNum >= m_pClause->m_vectorConjs.size()))
			return E_FAIL;

		int iConj;
		iConj = m_pClause->m_vectorConjs[iNum].m_index;

		switch(m_pClause->m_vectorConjs[iNum].m_FromWhere)
		{
		case FROM_OBOR_DIC:
			{
				strcpy(strBuf, GetOpt()->m_pOborDic->m_Entries[iConj].m_OborotEntryStr.c_str());				
				break;
			}
		case FROM_SUB_CONJ:
			{
				strcpy(strBuf,GetOpt()->m_pOborDic->GetSubConjs()[iConj].c_str());
				break;
			}
		
		}
			
		*pVal = _bstr_t(strBuf).copy();

	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;

}

