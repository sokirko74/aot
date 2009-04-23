// COMHomonym.cpp : Implementation of CCOMHomonym
#include "stdafx.h"
#include "SynAn.h"
#include "comdef.h"
#include "COMHomonym.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMHomonym


STDMETHODIMP CCOMHomonym::get_LemSign(BSTR *pVal)
{
	try
	{
		string s;
		if (m_pHomonym->m_LemSign)
			s += m_pHomonym->m_LemSign;
		s += m_pHomonym->m_CommonGramCode;
		*pVal = _bstr_t(s.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}




STDMETHODIMP CCOMHomonym::get_Poses(long *pVal)
{
	try
	{
		*pVal = m_pHomonym->m_iPoses;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMHomonym::get_Lemma(BSTR *pVal)
{
	try
	{
		*pVal = _bstr_t(m_pHomonym->m_strLemma.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CCOMHomonym::get_Grammems(hyper *pVal)
{
	try
	{
		*pVal = m_pHomonym->m_iGrammems;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}


STDMETHODIMP CCOMHomonym::get_GramDescriptionStr(BSTR *pVal)
{
	string s = m_pHomonym->GetGrammemsStr();

	*pVal = _bstr_t(s.c_str()).copy();
	return S_OK;
}

STDMETHODIMP CCOMHomonym::get_ParadigmID(long *pVal)
{
	*pVal = m_pHomonym->m_lPradigmID;
	return S_OK;
}

STDMETHODIMP CCOMHomonym::get_GramCodes(BSTR *pVal)
{
	try
	{
		*pVal = _bstr_t(m_pHomonym->m_GramCodes.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}





STDMETHODIMP CCOMHomonym::get_POSStr(BSTR *pVal)
{
	*pVal = _bstr_t(m_pHomonym->GetPartOfSpeechStr().c_str()).copy();
	return S_OK;
}

STDMETHODIMP CCOMHomonym::get_IsOb1(BOOL *pVal)
{
	*pVal = m_pHomonym->IsOb1();

	return S_OK;
}


STDMETHODIMP CCOMHomonym::get_IsOb2(BOOL *pVal)
{
	*pVal = m_pHomonym->IsOb2();

	return S_OK;
}

STDMETHODIMP CCOMHomonym::get_IsInOb(BOOL *pVal)
{
	*pVal = m_pHomonym->IsIsOb();

	return S_OK;
}


STDMETHODIMP CCOMHomonym::GetOborDictIdOfSimplePrep(long iNum, long *pVal)
{
	try
	{
		*pVal = -1;

		if( (iNum < 0) || (iNum >= m_pHomonym->m_SimplePrepNos.size()) )
			return E_FAIL;
		*pVal = m_pHomonym->m_SimplePrepNos[iNum];
	}
	catch(...)
	{
		return E_FAIL;		
	}	

	return S_OK;
}

STDMETHODIMP CCOMHomonym::get_SimplePrepsCount(long *pVal)
{
	try
	{
		*pVal = m_pHomonym->m_SimplePrepNos.size();
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}


STDMETHODIMP CCOMHomonym::GetOborotId(long *OborotId)
{
	try
	{
		*OborotId = m_pHomonym->m_OborotNo;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
	
}
STDMETHODIMP CCOMHomonym::GetOborotGF(BSTR* GF)
{
	try
	{
		*GF = _bstr_t("");
		if (m_pHomonym->m_OborotNo == -1) 
			return S_OK;
		*GF = _bstr_t(m_pHomonym->GetOborotPtr()->m_GrammarFeature.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
	
}
STDMETHODIMP CCOMHomonym::get_CommonGrammemsStr(BSTR* pVal)
{

	string s = m_pHomonym->GetOpt()->GetGramTab()->GrammemsToStr(m_pHomonym->m_TypeGrammems);
	*pVal = _bstr_t(s.c_str()).copy();
	return S_OK;
}
