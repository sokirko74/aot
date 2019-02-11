#include "stdafx.h"
#include "SemanticRusStructure.h"




void CRusSemStructure::FindPragmaticallyStrongSynRel () 
{
	for (long RelNo=0; RelNo< m_SynRelations.size(); RelNo++)
	{

		// ���� �� ���� ������� ����� ���� ������� � ���������� ����������,
		// ����� ����� ���������� ����� ��������
		if (m_SynRelations[RelNo].m_SynRelName.substr(0,5) == "�����")
		{
			vector<long> Rels;
			GetOutcomingSynRelations(m_SynRelations[RelNo].m_SourceNodeNo, Rels);
			long Count =0;
			for (long i=0;i < Rels.size(); i++) 
				if(m_SynRelations[Rels[i]].m_SynRelName.substr(0,5) == "�����")
					Count++;
			if (Count > 2)
				m_SynRelations[RelNo].m_bPragmaticallyStrong = true;
		};

	}
};	


bool CRusSemStructure::IsPragmaticallyStrongSynRel (long RelNo) const
{
	// ��������� �������� ������� �� � ������ � �������� � ��� �������,  ����� 
	// ������� ������ ������, ��� �� ��������� �������, ��������:
	// "� �� ����, ��� �� ����� �� ��� ���."
	// �� ��� ����� �������� "��" �� ����_���, ����� ����������� ������.
	if (m_Nodes[m_SynRelations[RelNo].m_TargetNodeNo].IsLemma("��"))
		return false;

	/*
	������������� ������������ "������" ����� ��������� ���� MUA, � �� ������ ���������  ����������� QUANTIT	
	*/
	if  (m_SynRelations[RelNo].m_SynRelName == "���_����_���")
	{
		if (   m_Nodes[m_SynRelations[RelNo].m_TargetNodeNo].IsWordForm("������")			
			|| m_Nodes[m_SynRelations[RelNo].m_TargetNodeNo].IsWordForm("������")  
			)
		{
			return false;
		};
	};


	return     !IsWeakSynRel(m_SynRelations[RelNo].m_SynRelName)
		|| m_SynRelations[RelNo].m_bPragmaticallyStrong;
};

void CRusSemStructure::ApplySynStr (long ClauseNo) 
{

	try	{
		for (long	j=0; j < m_Relations.size(); j++)
		{
			for (long l=0;	l <	m_SynRelations.size(); l++)
				if (IsPragmaticallyStrongSynRel(l) )
					if  (   (m_Relations[j].m_TargetNodeNo  == m_SynRelations[l].m_SourceNodeNo)
						&& (m_Relations[j].m_SourceNodeNo == m_SynRelations[l].m_TargetNodeNo)
						)
					{
						EraseRelation(j);
						j--;
						break;
					};
		};


		for (long	i=0; i < GetNodesSize(); i++)
			if	(	   IsInClause(i, ClauseNo)	
					&& m_Nodes[i].IsWordContainer()
				)
			{
				vector<long> SynRels;
				GetIncomingSynRelations(i, SynRels);

				for (long	l=0; l < SynRels.size(); l++)
				{
					
					if	(!IsPragmaticallyStrongSynRel(SynRels[l]) )	
						continue;

					string	 SynRelStr = m_SynRelations[SynRels[l]].m_SynRelName; 
					CRelSet R = GetIncomingRelations(i,  false);
					bool RelFound = false;
					for (long j=0;	j <	R.m_RelsCount; j++)
						if (m_Relations[R.m_Rels[j]].m_SourceNodeNo == m_SynRelations[SynRels[l]].m_SourceNodeNo)
						{
							RelFound	= true;
							m_Relations[R.m_Rels[j]].m_SyntacticRelation	= m_SynRelations[SynRels[l]].m_SynRelName;
							R.Erase(j);
							j--;
						};


					DeleteRelSet(R);
					if	(!RelFound)
					{
						if (!m_Nodes[m_SynRelations[SynRels[l]].m_SourceNodeNo].IsWordContainer())
							continue;

						string  SemRelStr;
						if (SynRelStr ==	"����_���")
							SemRelStr =	"QUANTIT";		   

						CRusSemRelation NewRelation(CValency(SemRelStr,	A_C), m_SynRelations[SynRels[l]].m_SourceNodeNo, i,	m_SynRelations[SynRels[l]].m_SynRelName);

						if	(			SynRelStr == "����_���" 
								||		SynRelStr == "����_���" 
								||		SynRelStr == "����_���" // ����� ���-������ ��� ��������... 	
							)
						{
							NewRelation.m_bReverseRel = true;
							NewRelation.m_Valency.m_RelationStr = "PROPERT";
						}
						assert (NewRelation.m_SourceNodeNo < m_Nodes.size());
						assert (NewRelation.m_TargetNodeNo < m_Nodes.size());
						AddRelation(NewRelation);
					};

				};





			};

	}
	catch (...)
	{
		ErrorMessage ("ApplySynStr");
		throw;
	};

};

