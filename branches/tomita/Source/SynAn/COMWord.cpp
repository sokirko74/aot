// COMWord.cpp : Implementation of CCOMWord
#include "stdafx.h"
#include "comdef.h"
#include "SynAn.h"
#include "COMWord.h"



STDMETHODIMP CCOMWord::get_WordStr(BSTR *pVal)
{
	try
	{
		_bstr_t b (m_pWord->m_strWord.c_str());
		*pVal = b.copy();

	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}


STDMETHODIMP CCOMWord::get_HomonymsCount(long *pVal)
{
	try
	{
		*pVal = m_pWord->m_Homonyms.size();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMWord::get_Homonym(long lHomNum, IHomonym **pVal)
{
	try
	{
		if( (lHomNum < 0) || (lHomNum >= m_pWord->m_Homonyms.size()) )
			return E_FAIL;

		CComObject<CCOMHomonym>* pHomonym = NULL;
		pHomonym = new CComObject<CCOMHomonym>;
		BOOL bRes = pHomonym->Init(m_pWord->m_Homonyms[lHomNum], m_pSent);
		if( !bRes )
		{
			delete pHomonym;
			return E_FAIL;
		}
		*pVal = pHomonym;
		pHomonym->AddRef();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMWord::get_IsInThesaurusEntry(BOOL *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_bInTermin;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}


STDMETHODIMP CCOMWord::get_Register(BOOL *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_Register;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	

	return S_OK;
}


STDMETHODIMP CCOMWord::get_ThesaurusEntryID(long *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_iTerminID;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	

	return S_OK;
}

STDMETHODIMP CCOMWord::get_IsLastInThesaurusEntry(BOOL *pVal)
{
		try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_bLastWordInTermin;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	


	return S_OK;
}

STDMETHODIMP CCOMWord::get_IsFirstInThesaurusEntry(BOOL *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_bFirstWordInTermin;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	


	return S_OK;
}

STDMETHODIMP CCOMWord::get_ThesType(int *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_ThesType;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	


	return S_OK;
}


IHomonym* CCOMWord::GetHomonymByClauseVar(CCOMClauseVariant* pVar)
{
	assert(pVar != NULL);
	if( pVar == NULL )
		return NULL;
	int iUnit = pVar->GetUnitNumByWordNum(m_iWordNum);
	if( iUnit == -1 )
		return NULL;

	long lHomNum;
	lHomNum = pVar->m_pSynVariant->m_SynUnits[iUnit].m_iHomonymNum;

	CComObject<CCOMHomonym>* pHomonym = NULL;
	pHomonym = new CComObject<CCOMHomonym>;

	CSynHomonym H (m_pSent);
	GetHomonymByClauseVariantWithModifiedProperties(iUnit, *m_pWord, *pVar->m_pSynVariant, H);
	pHomonym->Init(H, m_pSent);
	pHomonym->AddRef();
	return pHomonym;

}

STDMETHODIMP CCOMWord::get_HomonymByClauseVar(IClauseVariant* piClauseVariant, IHomonym **pVal)
{
	try
	{
		if( piClauseVariant == NULL)	
			return E_FAIL;

		assert(m_iWordNum != -1);
	
		CCOMClauseVariant* pVar = reinterpret_cast<CCOMClauseVariant*>(piClauseVariant);
		*pVal = GetHomonymByClauseVar(pVar);
		if( *pVal == NULL)
			return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMWord::get_ClauseNo(long *pVal)
{
	if( m_pWord == NULL )
		return E_FAIL;

	*pVal = m_pWord->m_iClauseNo;
	return S_OK;
}

STDMETHODIMP CCOMWord::get_GraphDescrs(BSTR *pVal)
{
	try
	{
		*pVal = _bstr_t(m_pWord->BuildGraphemDescr().c_str()).copy();

	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
	return S_OK;
}

STDMETHODIMP CCOMWord::get_bArtificialCreated(BOOL *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_bArtificialCreated;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	


	return S_OK;
}

STDMETHODIMP CCOMWord::get_bDeleted(BOOL *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_bDeleted;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	


	return S_OK;
}


STDMETHODIMP CCOMWord::get_MainVerbsCount(long *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_MainVerbs.size();
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	


	return S_OK;
}

STDMETHODIMP CCOMWord::get_MainVerbs(long lVerbNo, long *pVal)
{
	try
	{
		if( m_pWord == NULL )
			return E_FAIL;

		*pVal = m_pWord->m_MainVerbs[lVerbNo];
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}	


	return S_OK;
}

