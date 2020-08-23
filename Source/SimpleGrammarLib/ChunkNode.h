// QNode.h: interface for the CQNode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CHUNK_NODE_H
#define CHUNK_NODE_H


#include "../common/utilit.h"
#include "list"
#include "SimpleGrammar.h"


struct CNodeAttribute 
{
	std::string m_Name;
	std::string m_Value;
	CNodeAttribute(std::string Name,	std::string Value);
	std::string			GetStr() const;
};


struct CNodeAttributes {

	std::list<CNodeAttribute*> m_Items;
	~CNodeAttributes();
};



enum ChunkOperationEnum {stOr, stSequence, stFacultative};

class CChunkNode  
{
public:
	
	std::string					m_AtomicName;

	CNodeAttributes*		m_pAttributes;

	int						m_RefCount;

	CChunkNode();

	virtual ~CChunkNode();
	
	void			CreateAtomic(std::string AtomicName, CNodeAttributes* Attributes);
	virtual std::string			GetStr() const;

};


class CChunkSequenceNode : public CChunkNode
{
	std::vector<CChunkNode*> m_Children;
public:
	
	virtual ~CChunkSequenceNode();
	void Destroy();
	void					Create(CChunkNode* child1);
	void					AddChild(CChunkNode* child1);
	void					DeleteLastChild();
	CChunkSequenceNode*		Clone() const;
	const std::vector<CChunkNode*>&	GetChildren() const;
	virtual std::string						GetStr() const;
	
};



class CChunkRule {
public:
	
	CChunkNode*					m_pLeftHand;
	std::list<CChunkSequenceNode*>	m_pRightHand;
	std::vector<CRuleFeature>		m_FeatureExprs;
	int							m_SourceLineNo;
	std::string						m_SourceFileName;

	CChunkRule();
	CChunkRule(const CChunkRule& X);
	CChunkRule& operator= (const CChunkRule& X);
	~CChunkRule();
	
	bool	CreateRule(CChunkNode* child1);
	bool	AddRightHandMultipleNode(CChunkRule* pRule, ChunkOperationEnum Type);
	bool	MakeLastNodeFacultative();
	void	SetLeftHandNode(CChunkNode* child1);
	std::string	GetStr() const;
	bool	EncloseNodes();
	void	AddRuleFeatute(CRuleFeature* pFeature);
};


class CChunkGrammar {
	public: 
	std::list<CChunkRule*>	m_Rules;

	~CChunkGrammar();
	std::string		GetStr() const;
	void		AddRules(const CChunkGrammar& R);
};

#endif 
