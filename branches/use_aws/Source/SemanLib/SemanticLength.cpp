#include "stdafx.h"
#include "SemanticRusStructure.h"



// ��������� ����� ��������� � �������������� ��������
// �������������, ��� ��������� RelNo - ��������������.

long CRusSemStructure::GetRelationLeapsCount (long RelNo)
{
	
	long FirstNode = m_Relations[RelNo].m_SourceNodeNo;
	long LastNode = m_Relations[RelNo].m_TargetNodeNo;
	assert (m_Nodes[FirstNode].m_ClauseNo == m_Nodes[LastNode].m_ClauseNo);
	assert (m_Nodes[FirstNode].m_ClauseNo < m_Clauses.size());
	const CRusSemClause& C = m_Clauses[m_Nodes[FirstNode].m_ClauseNo];
	
	assert (FirstNode < m_Nodes.size());
	assert (LastNode < m_Nodes.size());
	if (m_Nodes[FirstNode].GetMinWordNo() > m_Nodes[LastNode].GetMinWordNo()) 
		swap (FirstNode, LastNode);


	long LeapCount = 0;
	assert ( LastNode < C.m_EndNodeNo );
	for (long NodeNo = FirstNode+1;NodeNo < LastNode; NodeNo++)
	{
		// �������� ������ ������� ������� ���� NodeNo, ���� �� �������� � ���� NodeNo ���������
		// ������ �� �������  �������������� ���������
		NodeNo = GetDefiniteRightHostByNotWeakSynRelation (NodeNo); 
		assert (NodeNo <=  C.m_EndNodeNo);
		if (NodeNo < LastNode)
			LeapCount++;
	};

	return LeapCount;
};


void CRusSemStructure::BuildRelationLeapsCount(long ClauseNo)
{
	try 
	{

		for (long i=0; i < m_Relations.size();i++)
			if (		IsInClause(m_Relations[i].m_SourceNodeNo, ClauseNo) 
				&&	IsInClause(m_Relations[i].m_TargetNodeNo, ClauseNo) 
				)
				if (IsWordContainerRel(i))
				{
					m_Relations[i].m_LeapsCount  = GetRelationLeapsCount (i);

				}
	}
	catch (...)
	{
		ErrorMessage ("BuildRelationLeapsCount");
		throw;
	};


};


void CRusSemStructure::DeleteLongRelations (long ClauseNo, long MaxRelationLeapsCount, bool bOnlyNouns)
{
 for (long i=0; i < m_Relations.size();i++)
  if (IsInClause(m_Relations[i].m_SourceNodeNo, ClauseNo))
    if (!bOnlyNouns || HasRichPOS (m_Relations[i].m_SourceNodeNo, NOUN) )
	  if (   (m_Nodes[m_Relations[i].m_SourceNodeNo].m_NodeType  != MNA)
		  && (m_Nodes[m_Relations[i].m_TargetNodeNo].m_NodeType != MNA)
		 )
		 if (m_Relations[i].m_Valency.IsFromDict() )
		   if (!bOnlyNouns || m_Relations[i].m_Valency.m_RossHolder->IsGenitiv(m_Relations[i].m_SynReal.m_Cortege.m_DomItemNos[1]))
			   if ( m_Relations[i].m_LeapsCount > MaxRelationLeapsCount)
			   {
			     EraseRelation(i);
				 i--;
			   };
};


long CRusSemStructure::GetRelationsLength(long Tag) const 
{
  size_t Result =0;

  long Count  = m_Relations.size();

  for (size_t i = 0;  i < Count; i++)
   if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
     if (m_Relations[i].m_bRelUse)
	 /*�� ����� ���������  �����  ���������, ������  � ��������� ����:
		1. ����������� ���� (��� �������� �� ����);
		2. ������� ����� (��� ������ ����� � ������ ������ � ������������ ���
		������. �����. � ���� ��, ����� ����������� ������ �� �����������)
	 */
	   if (  !m_Nodes[m_Relations[i].m_TargetNodeNo].IsAbstract() 
		   &&!m_Nodes[m_Relations[i].m_TargetNodeNo].m_bConjWord 
		  )
	   {
       	   long SourceNodeNo =  m_Relations[i].m_SourceNodeNo;
		   // ������������ �������� ���� Copul, ���� �  ����  ������ ������ ���� �������
			 if (   (m_Nodes[m_Relations[i].m_SourceNodeNo].m_NodeType == Copul)
				 && (m_Nodes[m_Relations[i].m_SourceNodeNo].m_InRels.size() == 1)
				)
			  SourceNodeNo = m_Relations[m_Nodes[m_Relations[i].m_SourceNodeNo].m_InRels[0]].m_SourceNodeNo;
			 else
				 if (m_Nodes[m_Relations[i].m_SourceNodeNo].IsAbstract() ) 
					 continue;

		   long u1 = SourceNodeNo;
		   long u2 = m_Relations[i].m_TargetNodeNo;
		   if (u2 < u1)  swap (u1, u2);
		   Result +=  u2 - u1;
       
	   };

  return Result; 
};
