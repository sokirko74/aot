#ifndef LR_items_h
#define LR_items_h


struct CWorkRule;
class CWorkGrammar;

struct CTableItem 
{
	int					m_GotoState;
	const CWorkRule*    m_pReduceRule;
	CTableItem ()
	{
		m_pReduceRule = 0;
		m_GotoState = -1;
	};
};

struct CLRItem 
{
	const CWorkRule*    m_pRule;
	size_t				m_DotSymbolNo;

	bool operator < (const CLRItem& X) const
	{
		if (m_DotSymbolNo != X.m_DotSymbolNo)
			return m_DotSymbolNo < X.m_DotSymbolNo;
		return m_pRule < X.m_pRule;
	};
	bool operator == (const CLRItem& X) const
	{
		return		m_DotSymbolNo == X.m_DotSymbolNo
				&&	m_pRule == X.m_pRule;
	};

	bool IsFinished() const;
	int GetSymbolNo() const;
};

struct CStateAndSymbol 
{
	int		m_SymbolNo;
	size_t	m_StateNo;

	bool operator < (const CStateAndSymbol& X) const
	{
		if (m_SymbolNo != X.m_SymbolNo)
			return m_SymbolNo < X.m_SymbolNo;
		else
			return m_StateNo < X.m_StateNo;
	}
};

struct CLRCollectionSet  
{
	//  item sets  which can be generated from the grammar
	vector< set<CLRItem> > m_ItemSets;

	// goto function m_GotoFunction[i] = k means that from 
	//  Itemset i.m_StateNo we can go to Itemset k over symbol i.m_SymbolNo.
	map< CStateAndSymbol, size_t> m_GotoFunction;

	void Compute(const CWorkGrammar* pWorkGrammar);
	bool GetGotoValue(int SymbolNo, size_t StateNo, size_t& ResultStateNo) const;

};


struct CInputPeriod {
	size_t m_Start;
	size_t m_End;
};

struct CLR0OutputItem  : public CInputPeriod
{	
	const CWorkRule*	m_pRule;
};




#endif
