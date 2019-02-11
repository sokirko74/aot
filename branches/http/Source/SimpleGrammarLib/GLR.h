#ifndef GLR_h
#define GLR_h

#include "GLRTable.h"
#include "InputSymbol.h"


struct CParsingChildrenSet
{
	const CGLRRuleInfo*	m_ReduceRule;
	vector<size_t>		m_Items;
};

struct CSymbolNode 
{
	//  a reference to a grammar
	CInputSymbol	m_Symbol;


	//  a reference to all state nodes which are successors of this symbol node
	vector<size_t>  m_NodeChildren;

	// m_ParseChildren[i] is one possible set of children of this symbol node in the parse tree
	// if m_ParseChildren.size ()>1, then  "local ambiguity packing" has occurred. 
	// if m_ParseChildren.empty(), then this node is a leaf in the parsing tree.
	vector<CParsingChildrenSet>	m_ParseChildren;

	// the period of the input stream, which is occupied by this symbol
	size_t	m_InputStart;
	size_t	m_InputEnd;

	// m_RecurseParentNodeNo is a redundant slot which poits to the parent of this symbol node
	// it can be calculated by the CStateNodem_SymbolChildren

	int		FindChild(size_t NodeNo) const 
	{
		vector<size_t>::const_iterator it = find (m_NodeChildren.begin(),m_NodeChildren.end(), NodeNo);
		if (it == m_NodeChildren.end()) return -1;
		return  it - m_NodeChildren.begin();
	};
};

struct CStateNode 
{
	int							m_StateNo;
	vector<size_t>				m_SymbolChildren;

};

struct CPendingReduction 
{
	size_t				m_SymbolNodeNo;
	size_t				m_LastStateNodeNo;
	const CGLRRuleInfo* m_pReduceRule;

	bool operator < (const CPendingReduction& X) const
	{
		if (m_SymbolNodeNo != X.m_SymbolNodeNo)
			return m_SymbolNodeNo < X.m_SymbolNodeNo;
		else
		if (m_LastStateNodeNo != X.m_LastStateNodeNo)
			return m_LastStateNodeNo < X.m_LastStateNodeNo;
		else
			return m_pReduceRule < X.m_pReduceRule;
	}

};

struct CPendingShift 
{
	
	set<CInputSymbol>::const_iterator	m_pSymbol;
	size_t								m_StateNodeNo;
	size_t								m_NextStateNo;

	bool operator < (const CPendingShift& X) const
	{
		if (m_StateNodeNo != X.m_StateNodeNo)
			return m_StateNodeNo < X.m_StateNodeNo;

		if (m_NextStateNo != X.m_NextStateNo)
			return m_NextStateNo < X.m_NextStateNo;

		return *m_pSymbol < *X.m_pSymbol;
	}
};

class CGLRParser  
{
	// m_InputLevels[i] is the upper border of m_StateNodes , so that all nodes  
	// [0..m_InputLevels[i]) from m_StateNodes were created for the input sequence till the i-th symbol
	// we created on the step i for the symbol i.
	vector< size_t >		m_InputLevels;

	// m_PendingReductions is a set of pending reduction
	set<CPendingReduction>	m_PendingReductions;

	// m_PendingShifts is a set of pending shifts
	set<CPendingShift>		m_PendingShifts;

	// m_ActiveNodes is a set of active state nodes
	set<size_t>				m_ActiveNodes;

	// Graph Structured Stack
	vector<CStateNode>		m_StateNodes;
	

	

	//void	Actor(size_t SymbolNo);
	//void	Reducer(size_t SymbolNo);
	//void	Shifter(size_t SymbolNo);
	void	Actor(const set<CInputSymbol>& Symbols);
	void	Reducer(const set<CInputSymbol>& Symbols);
	void	Shifter(const set<CInputSymbol>& Symbols);
	void	ReduceOnePath(const set<CInputSymbol>& Symbols, const CPendingReduction& Reduction, const vector<size_t>& Path);

	bool	CreateOrReuseStateNode(int StateNo, size_t& NewNodeNo);
	bool	HasPathOfLengthTwo(int SourceNodeNo, const CSymbolNode& BetweenNode, size_t& SymbolNodeNo) const;
	void	UpdatePendingReductions(const CSLRCellWork& C);
	size_t	GetFirstNodeNoOfCurrentClosure() const;
	
	size_t	GetNumberOfClosureSet(size_t StateNodeNo) const;
	bool	GetGrammarFeatures(const CPendingReduction& Reduction, const vector<size_t>& Path, CSymbolNode& Node);
	inline CSymbolNode& GetNodeFromPath(const vector<size_t>& Path, size_t RuleItemNo);
public: 
	bool					m_bRobust;
	const	CGLRTable*		m_pTable;
	vector<CSymbolNode>		m_SymbolNodes;

	CGLRParser();
	void InitGLRParser(const CGLRTable*	pTable);
	void CopyFrom(const CGLRParser& C);
	//bool ParseSymbol(size_t SymbolNo);
	bool ParseSymbol(const set<CInputSymbol>& Symbols);
	string GetDotStringOfStack(bool bShowStates) const;
	void DumpParser(bool bShowStates) const;
	bool	HasGrammarRootAtStart() const;
	string GetDotStringOfSymbolNode(size_t SymbolNodeNo) const;  
	string GetDotStringOfStackRecursive(size_t SymbolNodeNo) const;
	void	GetParseNodesRecursiveWithoutSyntaxAmbiguity(size_t SymbolNodeNo, vector<size_t>& Nodes, bool bShouldBeLeaf) const;
	void	GetParseNodesRecursive(size_t SymbolNodeNo, vector< set<size_t> >& Nodes, bool bShouldBeLeaf, size_t  RootInputStart) const;
	size_t	GetMainWordRecursive(size_t SymbolNodeNo)  const;
};

#endif
