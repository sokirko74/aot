#include "stdafx.h"
#include "SemanticRusStructure.h"


void CRusSemStructure::GetIncomingSynRelations (long NodeNo, vector<long>& Relations) const 
{
  Relations.clear();
  for (size_t i = 0; i<m_SynRelations.size(); i++)
     if (m_SynRelations[i].m_TargetNodeNo == NodeNo)
	    Relations.push_back(i);

};



bool CRusSemStructure::HasIncomingNotWeakSynRelation (long NodeNo) const 
{
	vector<long> Relations;
	GetIncomingSynRelations(NodeNo, Relations);
    for (long i=0; i < Relations.size(); i++)
	  if ( !IsWeakSynRel(m_SynRelations[Relations[i]].m_SynRelName) )
		  return true;
	return false;
};

long  CRusSemStructure::GetDefiniteRightHostByNotWeakSynRelation (long NodeNo) const 
{
	try
	{
		if (NodeNo >= m_Nodes.size()) return -1;
		for (;;)
		{
			vector<long> Relations;
			GetIncomingSynRelations(NodeNo, Relations);
			long i=0;
			for (; i < Relations.size(); i++)
				if ( !IsWeakSynRel(m_SynRelations[Relations[i]].m_SynRelName) )
					if (GetNode(m_SynRelations[Relations[i]].m_TargetNodeNo).GetMinWordNo() 
						< GetNode(m_SynRelations[Relations[i]].m_SourceNodeNo).GetMinWordNo())
						break;
			if (i == Relations.size()) break;
			NodeNo = m_SynRelations[Relations[i]].m_SourceNodeNo;
		};

		return NodeNo;
	}
	catch (...)
	{
		ErrorMessage ("GetDefiniteRightHostByNotWeakSynRelation");
		throw;
	};

};


long CRusSemStructure::GetSynRelationsCount (long NodeNo) const 
{
  long  Result = 0;
  for (size_t i = 0; i<m_SynRelations.size(); i++)
     if (m_SynRelations[i].m_TargetNodeNo == NodeNo)
	    Result++;
     else 
	  if (m_SynRelations[i].m_SourceNodeNo == NodeNo)
		  Result++;

 return Result;
};


long CRusSemStructure::GetSynHost (long NodeNo) const 
{
  vector<long> Rels;
  GetIncomingSynRelations (NodeNo, Rels);
  if (Rels.size() == 0)
	  return -1;
  return m_SynRelations[Rels[0]].m_SourceNodeNo;
};

void CRusSemStructure::GetOutcomingSynRelations (long NodeNo, vector<long>& Relations) const 
{
  Relations.clear();
  for (size_t i = 0; i<m_SynRelations.size(); i++)
     if (m_SynRelations[i].m_SourceNodeNo == NodeNo)
	    Relations.push_back(i);
};



void CRusSemStructure::DeleteSynRelations(vector<long>& Rels)
{
	if (Rels.size() == 0) return;
	sort (Rels.begin(), Rels.end());
	for (long i=Rels.size() - 1; i >= 0; i--)
		m_SynRelations.erase (m_SynRelations.begin() + Rels[i] );
};

void CRusSemStructure::DeleteSynRelationsByName(long NodeNo, string Name)
{
	for (size_t i = 0; i<m_SynRelations.size(); i++)
	 if  (   (m_SynRelations[i].m_SourceNodeNo == NodeNo)
		  || (m_SynRelations[i].m_TargetNodeNo == NodeNo) 
		 )
		 if (Name == m_SynRelations[i].m_SynRelName)
		 {
			 m_SynRelations.erase(m_SynRelations.begin() +i );
			 i--;
		 };
};

void CRusSemStructure::DeleteSynRelationsOfClause(long ClauseNo)
{
  for (size_t i = 0; i<m_SynRelations.size(); i++)
     if (m_Nodes[m_SynRelations[i].m_SourceNodeNo].m_ClauseNo == ClauseNo)
	 {
		 m_SynRelations.erase(m_SynRelations.begin() +i);
		 i--;
	 };
};

void CRusSemStructure::MoveIncomingSynRelations(long FromNode, long ToNode)
{
	if (FromNode == -1) return;
	if (ToNode == -1) return;

	for (long i=0; i<m_SynRelations.size(); )
	{
		if (m_SynRelations[i].m_TargetNodeNo == FromNode )
			m_SynRelations[i].m_TargetNodeNo = ToNode;

		// уничтожение петел, которые могли взяться из потсроения Timeнодов
		if (m_SynRelations[i].m_TargetNodeNo == m_SynRelations[i].m_SourceNodeNo)
			m_SynRelations.erase(m_SynRelations.begin() + i);
		else
		  i++;


	};

    
};

bool CRusSemStructure::HasSynRelation(long NodeNo, string RelationStr) const
{
	for (long i=0; i<m_SynRelations.size(); i++)
    if (m_SynRelations[i].m_TargetNodeNo == NodeNo)
		{ if (m_SynRelations[i].m_SynRelName == RelationStr) return true; }
     else 
	  if (m_SynRelations[i].m_SourceNodeNo == NodeNo)
		  { if (m_SynRelations[i].m_SynRelName == RelationStr) return true; }

	return false;
};
bool CRusSemStructure::HasOutcomingSynRelation(long NodeNo, string RelationStr) const
{
	for (long i=0; i<m_SynRelations.size(); i++)
    if (m_SynRelations[i].m_SourceNodeNo == NodeNo)
		 if (m_SynRelations[i].m_SynRelName == RelationStr) return true; 

	return false;
};

 
void CRusSemStructure::MoveSynRelations(long FromNode, long ToNode)
{
	if (FromNode == -1) return;
	if (ToNode == -1) return;

	for (long i=0; i<m_SynRelations.size(); )
	{
		if (m_SynRelations[i].m_TargetNodeNo == FromNode )
			m_SynRelations[i].m_TargetNodeNo = ToNode;

		if (m_SynRelations[i].m_SourceNodeNo == FromNode )
			m_SynRelations[i].m_SourceNodeNo = ToNode;

		// уничтожение петел, которые могли взяться из потсроения Timeнодов
		if (m_SynRelations[i].m_TargetNodeNo == m_SynRelations[i].m_SourceNodeNo)
			m_SynRelations.erase(m_SynRelations.begin() +i);
		else
		  i++;


	};

    
};





long CRusSemStructure::GetIncomingSynRelationsCount (long NodeNo) const
{
 vector<long> Relations;
 GetIncomingSynRelations(NodeNo, Relations);
 return Relations.size();
};

long CRusSemStructure::GetOutcomingSynRelationsCount (long NodeNo) const
{
 vector<long> Relations;
 GetOutcomingSynRelations(NodeNo, Relations);
 return Relations.size();
};
