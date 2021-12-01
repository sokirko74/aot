// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru), 
// ==========  Copyright by Alexey Sokirko (2004)


//   The main function of this header is CMorphAutomatBuilder::AddStringDaciuk 
//  This function is an implementation of algorithm for constructing minimal,
// deterministic, acyclic FSM from unordered std::set of std::strings, which was described 
// in  "Jan Daciuk, Stoyan Mihov, Bruce Watson, and Richard Watson, 
//      Incremental Construction of Minimal Acyclic Finite State Automata, 
//		Computational Linguistics, 26(1), March 2000."

#ifndef MorphAutomBuilder_h
#define MorphAutomBuilder_h

#include "MorphAutomat.h"



struct CTrieNodeBuild;
struct IsLessRegister: public std::less<CTrieNodeBuild*>
{
	bool operator ()(const CTrieNodeBuild* pNodeNo1, const CTrieNodeBuild* pNodeNo2) const;
};

typedef std::set<CTrieNodeBuild*, IsLessRegister> CTrieRegister;




struct CTrieNodeBuild
{
	bool						m_bFinal;
	int							m_IncomingRelationsCount;
	CTrieNodeBuild*				m_Children[MaxAlphabetSize];
	CTrieRegister::iterator		m_pRegister;
	bool						m_bRegistered;
	int							m_NodeId;
	BYTE						m_FirstChildNo;
	BYTE						m_SecondChildNo;
	

	void				Initialize();
	void				AddChild(CTrieNodeBuild* Child, BYTE ChildNo);
	void				ModifyChild(CTrieNodeBuild* Child, BYTE ChildNo, bool bUpdateIncoming);
	CTrieNodeBuild*		GetNextNode(BYTE RelationChar)  const;
	void				SetNodeIdNullRecursive ();
	void				UnregisterRecursive();
	

	//  debug function
	bool				CheckIncomingRelationsCountRecursive(std::map<const CTrieNodeBuild*, size_t>& Node2Incoming) const;
	void				GetIncomingRelationsCountRecursive(std::map<const CTrieNodeBuild*, size_t>& Node2Incoming) const;
	bool				CheckRegisterRecursive() const;
	void				SetFinal(bool bFinal);
};




class CMorphAutomatBuilder : public CMorphAutomat
{

	CTrieNodeBuild*			m_pRoot;

	// a hash by the letter of the first  child and the number of children
	CTrieRegister			m_RegisterHash[MaxAlphabetSize+1][MaxAlphabetSize+1];

	std::vector<CTrieNodeBuild*>				m_Prefix;

	std::vector<CTrieNodeBuild*>		m_DeletedNodes;
	
	void				ClearBuildNodes();

	CTrieNodeBuild*		CreateNode();
	void				DeleteNode(CTrieNodeBuild* pNode);
	CTrieNodeBuild*		CloneNode(const CTrieNodeBuild* pPrototype);

	void				UpdateCommonPrefix(const std::string& WordForm);
	CTrieNodeBuild*		AddSuffix(CTrieNodeBuild* pParentNodeNo, const char* WordForm);
	CTrieNodeBuild*		ReplaceOrRegister(CTrieNodeBuild* pNode);
	void				DeleteFromRegister(CTrieNodeBuild* pNode);
	int					GetFirstConfluenceState() const;
	void				UnregisterNode(CTrieNodeBuild* pNode);
	
	CTrieRegister&		GetRegister(const CTrieNodeBuild* pNode);

	//  debug functions 
	bool				CheckRegister() const;
	bool				IsValid() const;
	
	
public:
	CMorphAutomatBuilder(MorphLanguageEnum Language, BYTE AnnotChar);
	~CMorphAutomatBuilder();

	void	InitTrie();
	void	AddStringDaciuk(const std::string& WordForm);
	void	ClearRegister();
	void	ConvertBuildRelationsToRelations();
};



#endif
