// SemanticMarkup.h: interface for the CSemanticMarkup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEMANTICMARKUP_H__760F4B3D_7FBA_490E_A34D_A31F02CCAB0B__INCLUDED_)
#define AFX_SEMANTICMARKUP_H__760F4B3D_7FBA_490E_A34D_A31F02CCAB0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct SSemMeaning
{
	std::vector<std::string> m_values;
};

struct SSemInfo
{
	SSemInfo(){}
	SSemInfo(SSemMeaning& m)
	{
		m_Meanings.push_back(m);
	};

	std::vector<SSemMeaning> m_Meanings;
};

struct SLemma
{
	SLemma(const char* w, BYTE pos)
	{
		m_sLemma = w;
		m_POS = pos;
	}
	SLemma() {};

	std::string m_sLemma;
	BYTE m_POS;
	bool operator<(const SLemma& lem) const
	{
		if( m_POS != lem.m_POS )
			return m_POS < lem.m_POS;
		return m_sLemma < lem.m_sLemma;
	}
};

class CSemanticMarkup  
{
public:
	CSemanticMarkup();
	virtual ~CSemanticMarkup();
	bool Open(std::string strName);
	std::string GetSemFeaturesString(std::string& lemma, BYTE POS);

protected:
	BYTE GetPOS(std::string s);
	void ReadHeadLine(const char *ss);
	void ReadItems(const char *ss);
	std::vector<std::string> m_Titles;
	map<SLemma,SSemInfo> m_Entries;
};

#endif // !defined(AFX_SEMANTICMARKUP_H__760F4B3D_7FBA_490E_A34D_A31F02CCAB0B__INCLUDED_)
