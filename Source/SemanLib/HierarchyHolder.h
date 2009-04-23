#ifndef HierarchyHolder_h
#define HierarchyHolder_h

#include "RossHolder.h"

enum CHierarchyEnum {SemRel, SemFet};

class CStringRelation {
public:
	string	m_Source;
	string	m_Target;

	CStringRelation (const string& Source, const string& Target);
	bool operator == (const CStringRelation& X) const;
	bool operator < (const CStringRelation& X) const;
};
struct CNodePlace {
	int x;
	int y;
};

struct CHierarchyNode 
{
	string			m_Name;	  
	CNodePlace		m_Point;
	long ItemNo;


	CHierarchyNode (const string& Name, const CNodePlace& p);
	CHierarchyNode (const string& Name);
	const string& GetName() const;
};


struct CHierarchyRelation 
{
	size_t node1;
	size_t node2;
	bool bWork;
	string name;

	CHierarchyRelation ();
	CHierarchyRelation (size_t _node1,size_t _node2, string _name, bool _bWork);
	bool operator ==  (const CHierarchyRelation& X ) const;
};


class CHierarchyHolder 
{
public:
	vector<CHierarchyNode>     Nodes;
	vector<CHierarchyRelation> Relations;

	// строковый вариант транзитивного замыкания иерархии
	vector<CStringRelation>   m_TransitiveRels;
	CRossHolder*              m_pRossDoc;
	CDictionary*  GetRoss () {return m_pRossDoc->GetRoss();}
	const CDictionary*   GetRoss () const {return m_pRossDoc->GetRoss();}
	CHierarchyEnum    m_Type;

	void MySerialize(bool IsStoring, bool WithoutView);
	BYTE GetDomNo() const;  


	virtual void CreateNode(const char* name, CNodePlace p, bool WithoutView);
	virtual void CreateRelation(int node1, int node2, bool bWork, bool WithoutView);
	void CreateNode(const char* Name);
	void CreateRelation(int node1, int node2, bool bWork);
	int GetWorkParent(int NodeNo);

	size_t   FindNodeByName (const char* name) const;
	int     IncomingEdgesCount(int NodeNo) const;
	void     SetHierarchyTransitiveClosure();
	void     ReadFromRoss(bool WithoutView );
	void     WriteToRoss(string Entry);


	CHierarchyHolder(CRossHolder* pRossDoc = 0);
	~CHierarchyHolder();

};






extern bool SemFetActantIsEqualOrLower (CRossHolder* Ross, WORD Host, BYTE LeafId, BYTE BracketLeafId, const string& ItemStr, CHierarchyHolder* pHierarchyDoc);
// добавляет в SemFets все SF, которые стоят ниже по иерархии 
extern void  IncludeLowerInHierarchy (CHierarchyHolder* pHierarchyDoc, vector<string>& SemFets);
// добавляет в SemFets все SF, которые стоят выше по иерархии 
extern void  IncludeHigherInHierarchy(CHierarchyHolder* pHierarchyDoc, vector<string>& SemFets);


#endif
