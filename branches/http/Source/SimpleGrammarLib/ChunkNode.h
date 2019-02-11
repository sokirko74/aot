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
	string m_Name;
	string m_Value;
	CNodeAttribute(string Name,	string Value);
	string			GetStr() const;
};


struct CNodeAttributes {

	list<CNodeAttribute*> m_Items;
	~CNodeAttributes();
};



enum ChunkOperationEnum {stOr, stSequence, stFacultative};

class CChunkNode  
{
public:
	
	string					m_AtomicName;

	CNodeAttributes*		m_pAttributes;

	int						m_RefCount;

	CChunkNode();

	virtual ~CChunkNode();
	
	void			CreateAtomic(string AtomicName, CNodeAttributes* Attributes);
	virtual string			GetStr() const;

};


class CChunkSequenceNode : public CChunkNode
{
	vector<CChunkNode*> m_Children;
public:
	
	virtual ~CChunkSequenceNode();
	void Destroy();
	void					Create(CChunkNode* child1);
	void					AddChild(CChunkNode* child1);
	void					DeleteLastChild();
	CChunkSequenceNode*		Clone() const;
	const vector<CChunkNode*>&	GetChildren() const;
	virtual string						GetStr() const;
	
};



class CChunkRule {
public:
	
	CChunkNode*					m_pLeftHand;
	list<CChunkSequenceNode*>	m_pRightHand;
	vector<CRuleFeature>		m_FeatureExprs;
	int							m_SourceLineNo;
	string						m_SourceFileName;

	CChunkRule();
	CChunkRule(const CChunkRule& X);
	CChunkRule& operator= (const CChunkRule& X);
	~CChunkRule();
	
	bool	CreateRule(CChunkNode* child1);
	bool	AddRightHandMultipleNode(CChunkRule* pRule, ChunkOperationEnum Type);
	bool	MakeLastNodeFacultative();
	void	SetLeftHandNode(CChunkNode* child1);
	string	GetStr() const;
	bool	EncloseNodes();
	void	AddRuleFeatute(CRuleFeature* pFeature);
};


class CChunkGrammar {
	public: 
	list<CChunkRule*>	m_Rules;

	~CChunkGrammar();
	string		GetStr() const;
	void		AddRules(const CChunkGrammar& R);
};

#endif 
