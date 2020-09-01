#include "stdafx.h"
#include "Synan_i.h"
#include "comdef.h"
#include "COMHomonym.h"
#include "COMSentence.h"
#include "../SynCommonLib/SyntaxInit.h"
#include "../SynCommonLib/Word.h"


CCOMHomonym::CCOMHomonym()
{
	m_pHomonym = 0;
};

CCOMHomonym::~CCOMHomonym()
{
	if (m_pHomonym != 0)
		delete m_pHomonym;
};

BOOL CCOMHomonym::Init(const CSynHomonym& pHom, CSentence* pSent)
{
	m_pHomonym = new CSynHomonym(pSent);
	*m_pHomonym = pHom;
	m_pSent = pSent;
	return TRUE;
};


const CSyntaxOpt* CCOMHomonym::GetOpt() const
{
	return m_pSent->GetOpt();
};

STDMETHODIMP CCOMHomonym::get_LemSign(BSTR *pVal)
{
	try
	{
		std::string s;
		if (m_pHomonym->m_LemSign)
			s += m_pHomonym->m_LemSign;
		s += m_pHomonym->m_CommonGramCode;
		s = convert_to_utf8(s.c_str(), GetOpt()->m_Language);
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
		std::string s = m_pHomonym->m_strLemma;
		s = convert_to_utf8(s.c_str(), GetOpt()->m_Language);
		*pVal = _bstr_t(s.c_str()).copy();
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
	std::string s = m_pHomonym->GetGrammemsStr();
	s = convert_to_utf8(s.c_str(), GetOpt()->m_Language);

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
		std::string s = m_pHomonym->GetGramCodes();
		s = convert_to_utf8(s.c_str(), GetOpt()->m_Language);
		*pVal = _bstr_t(s.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}





STDMETHODIMP CCOMHomonym::get_POSStr(BSTR *pVal)
{
	std::string s = m_pHomonym->GetPartOfSpeechStr();
	s = convert_to_utf8(s.c_str(), GetOpt()->m_Language);
	*pVal = _bstr_t(s.c_str()).copy();
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
		std::string s = m_pHomonym->GetOborotPtr()->m_GrammarFeature;
		s = convert_to_utf8(s.c_str(), GetOpt()->m_Language);
		*GF = _bstr_t(s.c_str()).copy();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
	
}
STDMETHODIMP CCOMHomonym::get_CommonGrammemsStr(BSTR* pVal)
{

	std::string s = m_pHomonym->GetOpt()->GetGramTab()->GrammemsToStr(m_pHomonym->m_TypeGrammems);
	s = convert_to_utf8(s.c_str(), GetOpt()->m_Language);
	*pVal = _bstr_t(s.c_str()).copy();
	return S_OK;
}
