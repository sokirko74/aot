#pragma once 

#include <string>

struct CInputSymbol 
{
	size_t	m_GrammarSymbolNo;
	std::string	m_GramCodes;
	std::string	m_TypeGramCodes;
	bool	m_bDeterm;	
	bool	m_bClause;	

	CInputSymbol();
	CInputSymbol(size_t GrammarSymbolNo, std::string GramCodes, std::string TypeGramCodes);
	bool operator < (const CInputSymbol& X) const;
	bool operator == (const CInputSymbol& X) const;
	void CopyAttributes(const CInputSymbol& X);
	const std::string& GetGrmInfoByAttributeName(const std::string& AttributeName) const;
};
