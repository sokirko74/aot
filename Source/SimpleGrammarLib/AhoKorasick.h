#ifndef AhoKorasick_h
#define AhoKorasick_h


#include "../common/utilit.h"



typedef int TerminalSymbolType;
typedef std::vector<TerminalSymbolType> TerminalString;
typedef  TerminalString::const_iterator CTSI;
typedef  TerminalString::iterator TPI;
typedef std::map<TerminalSymbolType, std::string> SymbolInformationType;  

struct	CTrieNode;
struct	CWorkRule;
class	CGLRParser;



struct COccurrence : public std::pair<size_t, size_t>
{
	// a copy from   CPattern::m_GrammarRuleNo
	size_t							m_GrammarRuleNo;

	CGLRParser*						m_pParser;
};


struct CTrieNode 
{
	int						m_FailureFunction;
	int						m_Parent;
	TerminalSymbolType		m_IncomingSymbol;
	int						m_GrammarRuleNo;
	DWORD					m_ChildrenIndex;
	BYTE					m_Depth;
	CTrieNode();
};

struct CTrieRelation 
{
	TerminalSymbolType	m_RelationChar;
	int					m_ChildNo;
};


class CTrieHolder 
{
	size_t							m_AlphabetSize;
	const SymbolInformationType*	m_pSymbolInformation;
	std::vector<CTrieNode>				m_Nodes;

	// m_ChildrenAux is used only while CTrieHolder::Create
	std::vector<int>						m_ChildrenAux;

	std::vector<CTrieRelation>			m_Children;


	std::vector<int>::iterator  GetChildrenAux(size_t NodeNo) { return m_ChildrenAux.begin() + NodeNo*GetAlphabetSize(); };
	std::vector<int>::const_iterator  GetChildrenAux(size_t NodeNo) const { return m_ChildrenAux.begin() + NodeNo*GetAlphabetSize(); };

	void CreateChildrenSequence(CTSI begin, CTSI end, size_t ParentNo, size_t WorkRuleNo);
	void ConvertAuxChildrenToNormal();
	int  GetTerminatedPeriodNext (int NodeNo) const;
	void UpdatePossibleOutputSymbolsbyOnState(size_t NodeNo,  std::vector<bool>& PossibleOutputSymbols) const;
	void CreateTrie(const std::set< CWorkRule >& Patterns);
	void AddNode(const CTrieNode& T);

public:
	
	//  creating
	void	InitFailureFunction();
	void	Create(const std::set< CWorkRule >& Patterns, const SymbolInformationType* Info);
	bool	Save(std::string GrammarFileName) const;

	//  loading 
	bool	Load(const SymbolInformationType* Info, std::string GrammarFileName);
	

	//  access members
	size_t	GetAlphabetSize() const;
	void	GetOccurrences (const TerminalString& Text, std::vector< COccurrence >& Occurrences) const;
	void	PrintChildren(size_t NodeNo) const;
	void	UpdatePossibleOutputSymbols (const std::set<size_t>& CurrentStates, std::vector<bool>& PossibleOutputSymbols) const;
	int		NextState (int State, size_t TextPositionNo, TerminalSymbolType Symbol, std::vector< COccurrence >& Occurrences) const;
	size_t	GetChildrenCount(size_t NodeNo)  const;
	int		FindChild(int NodeNo, TerminalSymbolType Child) const;
	std::vector<CTrieRelation>::const_iterator  GetChildren(size_t NodeNo) const;



};


extern void SolveAmbiguity (std::vector< COccurrence >& Occurrences);


#endif
