#ifndef InputSymbol_h
#define InputSymbol_h

struct CInputSymbol 
{
	size_t	m_GrammarSymbolNo;
	string	m_GramCodes;
	string	m_TypeGramCodes;
	bool	m_bDeterm;	
	bool	m_bClause;
    bool    m_bRelativeClause;
    size_t	m_ClauseType;

	CInputSymbol() {	};
	CInputSymbol(size_t GrammarSymbolNo, string GramCodes, string TypeGramCodes)
	{
		m_GrammarSymbolNo = GrammarSymbolNo;
		m_GramCodes = GramCodes;
		m_TypeGramCodes = TypeGramCodes;
		m_bDeterm = false;
		m_bClause = false;
        m_bRelativeClause = false;
        m_ClauseType = UnknownSyntaxElement;
	}

	bool operator < (const CInputSymbol& X) const
	{
		if (m_GrammarSymbolNo != X.m_GrammarSymbolNo)
			return m_GrammarSymbolNo < X.m_GrammarSymbolNo;

		if (m_bDeterm != X.m_bDeterm)
			return m_bDeterm < X.m_bDeterm;

		if (m_bClause != X.m_bClause)
			return m_bClause < X.m_bClause;
        
        if (m_bRelativeClause != X.m_bRelativeClause)
			return m_bRelativeClause < X.m_bRelativeClause;

        if (m_ClauseType != X.m_ClauseType)
			return m_ClauseType < X.m_ClauseType;

		if (m_TypeGramCodes !=  X.m_TypeGramCodes)
			return m_TypeGramCodes <  X.m_TypeGramCodes;

		return m_GramCodes < X.m_GramCodes;
	}

	bool operator == (const CInputSymbol& X) const
	{
		return		(m_GrammarSymbolNo == X.m_GrammarSymbolNo)
				&&	(m_bDeterm == X.m_bDeterm)
				&&	(m_bClause == X.m_bClause)
                &&	(m_bRelativeClause == X.m_bRelativeClause)
                &&	(m_ClauseType == X.m_ClauseType)
				&&	(m_GramCodes == X.m_GramCodes)
				&&	(m_TypeGramCodes == X.m_TypeGramCodes)
				;
	}
	void CopyAttributes(const CInputSymbol& X)
	{
		m_GramCodes = X.m_GramCodes;
		m_TypeGramCodes = X.m_TypeGramCodes;
		m_bDeterm = X.m_bDeterm;
		m_bClause = X.m_bClause;
        m_bRelativeClause = X.m_bRelativeClause;
        m_ClauseType = X.m_ClauseType;
	};

	const string& GetGrmInfoByAttributeName (const string& AttributeName) const
	{
		if (AttributeName == "grm")
			return m_GramCodes;
		else
		if (AttributeName == "type_grm")
			return m_TypeGramCodes;
		else
		{
			assert (false);
			return m_GramCodes;
		}
	};
};


#endif
