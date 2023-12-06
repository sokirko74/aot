#include "InputSymbol.h"
#include "assert.h"


CInputSymbol::CInputSymbol() {	

};

CInputSymbol::CInputSymbol(size_t GrammarSymbolNo, std::string GramCodes, std::string TypeGramCodes)
{
	m_GrammarSymbolNo = GrammarSymbolNo;
	m_GramCodes = GramCodes;
	m_TypeGramCodes = TypeGramCodes;
	m_bDeterm = false;
	m_bClause = false;
}

bool CInputSymbol::operator < (const CInputSymbol& X) const
{
	if (m_GrammarSymbolNo != X.m_GrammarSymbolNo)
		return m_GrammarSymbolNo < X.m_GrammarSymbolNo;

	if (m_bDeterm != X.m_bDeterm)
		return m_bDeterm < X.m_bDeterm;

	if (m_bClause != X.m_bClause)
		return m_bClause < X.m_bClause;

	if (m_TypeGramCodes != X.m_TypeGramCodes)
		return m_TypeGramCodes < X.m_TypeGramCodes;

	return m_GramCodes < X.m_GramCodes;
}

bool CInputSymbol::operator == (const CInputSymbol& X) const
{
	return		(m_GrammarSymbolNo == X.m_GrammarSymbolNo)
		&& (m_bDeterm == X.m_bDeterm)
		&& (m_bClause == X.m_bClause)
		&& (m_GramCodes == X.m_GramCodes)
		&& (m_TypeGramCodes == X.m_TypeGramCodes)
		;
}

void CInputSymbol::CopyAttributes(const CInputSymbol& X)
{
	m_GramCodes = X.m_GramCodes;
	m_TypeGramCodes = X.m_TypeGramCodes;
	m_bDeterm = X.m_bDeterm;
	m_bClause = X.m_bClause;
};

const std::string& CInputSymbol::GetGrmInfoByAttributeName(const std::string& AttributeName) const
{
	if (AttributeName == "grm")
		return m_GramCodes;
	else
		if (AttributeName == "type_grm")
			return m_TypeGramCodes;
		else
		{
			assert(false);
			return m_GramCodes;
		}
};

