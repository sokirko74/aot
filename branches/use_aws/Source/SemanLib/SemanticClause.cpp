#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "HierarchyHolder.h"


// ===== ��������� ������ � �������


//������� ���� ����������� ��������� ����� ClauseNo1 � ClauseNo2
bool CRusSemStructure::CreateClauseRelation(long ClauseRuleNo, long ClauseNo1, long ClauseNo2, CValency V, long TargetNodeNo, CDictUnitInterp Conj, long SourceNodeNo)
{
	if (SourceNodeNo == -1)
	{

		vector<long> Nodes1;
		GetClauseRootsWithoutDeleted(ClauseNo1,Nodes1);

		// ���� ��� ����, �� �������, ��� �� ������ ����� ����� ����, ������� �� ����������
		if (Nodes1.size() == 2)
			SourceNodeNo = Nodes1[1];
		else
			if (Nodes1.size() != 1) 
				return false;
			else
				SourceNodeNo = Nodes1[0];
	};

	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	if (Nodes2.size() == 2)  
	{

		vector<long>::iterator It = find (Nodes2.begin(), Nodes2.end(), TargetNodeNo);
		if (It == Nodes2.end()) return false;
		Nodes2.erase(It);
		AddRelation(CRusSemRelation(V, SourceNodeNo, Nodes2[0],   ""));
		m_Nodes[TargetNodeNo].m_bToDelete = true;
	}
	else 
		if (Nodes2.size() == 1) 
		{
			AddRelation(CRusSemRelation(V, SourceNodeNo, TargetNodeNo,  "" ));
		}
		else 
			return false;

	m_Relations[m_Relations.size()-1].m_SynReal.m_Conj = Conj;
	m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;


	return true;
};

long CRusSemStructure::GetClauseRelationCount () {

	long Result = 0;

	for (long i=0; i < m_Relations.size(); i++)
		if (IsRelBetweenClauses(m_Relations[i]))
			Result ++;

	return Result;
};

bool CRusSemStructure::IsEmptyClause(long ClauseNo)
{
	for (long i=0; i <m_Nodes.size(); i++)
     if (IsInClause(i, ClauseNo))
		 return false;

	 return true;
};



void CRusSemStructure:: DelClause(long ClauseNo)
{
  for (long i=0; i <m_Nodes.size(); i++)
  {
	  if (m_Nodes[i].m_ClauseNo > ClauseNo)
		  m_Nodes[i].m_ClauseNo--;
	  else
	  if (m_Nodes[i].m_ClauseNo == ClauseNo)
		  DelNode(i);
  };
  // ����� ������� � ������� m_SynClauseRels
  for (long k=0; k<m_SynClauseRels.size(); k++)
  {

	  if  (      (m_SynClauseRels[k].m_TargetNodeNo == ClauseNo)
		  || (m_SynClauseRels[k].m_SourceNodeNo == ClauseNo)
		  )
	  {
		  m_SynClauseRels.erase(m_SynClauseRels.begin() +k);
		  k--;
		  continue;
	  };


	  if (m_SynClauseRels[k].m_SourceNodeNo > ClauseNo)    m_SynClauseRels[k].m_SourceNodeNo--;
	  if (m_SynClauseRels[k].m_TargetNodeNo > ClauseNo) m_SynClauseRels[k].m_TargetNodeNo--;
  };
  m_Clauses.erase(m_Clauses.begin() + ClauseNo);
};

void CRusSemStructure:: DeleteEmptyClauses()
{
// ������� ��� ������ ������
    for (long ClauseNo =0; ClauseNo < m_Clauses.size(); ClauseNo++)
	 if (IsEmptyClause(ClauseNo))
	 {
		 DelClause(ClauseNo);
		 ClauseNo --;
	 };
};




bool CRusSemStructure:: ClauseHasCompAdj (long ClauseNo) const
{
	for (size_t i=0; i< m_Nodes.size(); i++)
		if (    IsInClause(i, ClauseNo) 
		 	 &&  (    IsCompAdj(i)
			      || HasSynRelation (i, "����_�����")
				  || (     HasRichPOS(i, NUMERAL)
					   && m_Nodes[i].HasOneGrammem (rComparative)
				     )
				 )
		   )
		   return true;

    return false;
};

bool CRusSemStructure:: ClauseHasChem(long ClauseNo) const
{
	for (size_t i=0; i< m_Nodes.size(); i++)
		if (    IsInClause(i, ClauseNo) 
		 	 && m_Nodes[i].IsPrimitive()
			 && ( m_Nodes[i].m_Words[0].m_Lemma == "���")
		   )
		   return true;

    return false;
};



bool CRusSemStructure:: ClauseHasNegation (long ClauseNo) const
{
	for (size_t i=0; i< m_Nodes.size(); i++)
		if (    IsInClause(i, ClauseNo) 
			 && m_Nodes[i].IsPrimitive()
		 	 && (m_Nodes[i].m_Words[0].m_Lemma == "��")
		   )
		   return true;

    return false;
};

bool CRusSemStructure:: ClauseHasNegationInVerb (long ClauseNo) const
{
	for (size_t i=0; i< m_Nodes.size(); i++)
		if (    IsInClause(i, ClauseNo) 
			 && m_Nodes[i].IsPrimitive()
		 	 && m_Nodes[i].HasRelOperator("��")
			 && IsVerbForm(m_Nodes[i])
		   )
		   return true;

    return false;
};


bool CRusSemStructure::IsConnectedClause(size_t ClauseNo)
{
  int LastNodeNo = -1;
  for (size_t i=0; i<m_Nodes.size(); i++)
   if  (m_Nodes[i].IsWordContainer())
   {
	  m_Nodes[i].m_bReached = !IsInClause(i, ClauseNo);
	  if (!m_Nodes[i].m_bReached)
		  LastNodeNo  = i;
   };

  if (LastNodeNo == -1) return true;

  dfs ((size_t)LastNodeNo, false);

  for (size_t i=0; i<m_Nodes.size(); i++)
	 if  (m_Nodes[i].IsWordContainer())
	  if (!m_Nodes[i].m_bReached) 
		  return false;

  return true;
};



void CRusSemStructure::GetClauseRootsWithoutDeleted (size_t ClauseNo, vector<long>& Nodes) const
{
	Nodes.clear();
	// �������� �� ���� ����� � ��������� ����, � ������� �� ������ �� ���� ����� 
	for (size_t i=m_Clauses[ClauseNo].m_BeginNodeNo; i<m_Clauses[ClauseNo].m_EndNodeNo; i++)
		if  (m_Nodes[i].m_IncomRelsCount  == 0)
			if (!m_Nodes[i].m_bToDelete)
				Nodes.push_back(i);

	/*�������� �� ���� ������, ���� ����� ���� �� ����, ������� ������� �� ��������,
	�����,  � ���� � ����, � ������� ������ ������ ���� ����� - ����� ��� ������� 
	*/
	for (size_t i=0; i<m_Relations.size(); i++)
		if (!m_Nodes[m_Relations[i].m_TargetNodeNo].m_bToDelete)
			if (m_Nodes[m_Relations[i].m_SourceNodeNo].m_bToDelete)
				if (IsInClause(m_Relations[i].m_TargetNodeNo, ClauseNo) )
					if (m_Nodes[m_Relations[i].m_TargetNodeNo].m_IncomRelsCount == 1)
						Nodes.push_back(m_Relations[i].m_TargetNodeNo);
};



/*
 "���� ��� �� ��������� � ������ �� ��������, ��� �� �������� ��� ���� ����������� �� emacs � ������ ������ ������, ���������� ������ control-g."
 ����� ����������� ����� ����� ������ � ������� ��������, ������ ��� 
 ������������ � ������, ������� � ��� ��� �������. ������
 GetClauseRootsWithoutDeleted �� ������ ������ ����� �������� ���� ������, � ��� �����,
 ����� �� ����� "���������"

*/
void GetRootsWithoutDeletedOrSyntaxTop(const CRusSemStructure& R, long ClauseNo, vector<long>& Nodes)
{
	R.GetClauseRootsWithoutDeleted(ClauseNo,Nodes);
	if (Nodes.size() == 0)
	{
		if (R.m_Clauses[ClauseNo].m_ClauseSyntaxTop != -1)
		{
			Nodes.push_back(R.m_Clauses[ClauseNo].m_ClauseSyntaxTop);
		};
	};

};



// ============ ������� ���������� ����� ==================


bool CRusSemStructure::TryClauseSubordConj(long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	/*
	"��� �� ���� �������, � ���� � �� ��������� �����, ��� ��������� ���� �������"
	����� ���� "�" ������������ � ���� m_bHasParticleConjAnd � ���������, ������� ��������� 
	��� ������������ ������������
	"��� �� ���� �������, ���� � �� ��������� �����, ��� ��������� ���� �������"
	���� ������, ������� ������ �����������, ����� ������ (���������� �������), �����
	���� � ��� ��������� ���� m_bHasParticleConjAnd, ����� 
	*/
	if (m_Clauses[ClauseNo2].m_bHasParticleConjAnd)
		if (ClauseNo1 < ClauseNo2)
			return false;

	const CDictUnitInterp* Conj =   GetSubordConjFromTheBegining(ClauseNo2);
	if (Conj == 0) return false;
	CValency V = GetSemRelOfPrepOrConj(*Conj);

	// ��� "���" ���� ��������� �������
	long NodeNo = FindFirstNode(ClauseNo2);
	if  (NodeNo == -1) return 0;
	assert (m_Nodes[NodeNo].m_Words.size() > 0);
	if (m_Nodes[NodeNo].m_Words[0].m_Lemma == "���") return false;

	// ��������� ������� ������ ������, ���������� �� �������� � ����� ���� � GF2 ����� 
	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	if (Nodes2.size() != 2)  return false;
	CSemPattern P;
	P.InitSemPattern(GetRossHolder(Conj->m_DictType), Conj->m_UnitNo, 2, 0);
	if (!P.LoadGramFromDict()) return false;

	long l=0;
	for (; l < P.m_GramCorteges.size();l++)
	{
		string SynRel = P.GetSynRelStr(l);

		if  (    (SynRel == "���") 
			&& IsInfinitiveOrMNAInfinitive(Nodes2[1])
			) 
			break;

		if  (    (SynRel == "��_���") 
			&& m_Nodes[Nodes2[1]].HasOneGrammem( rPastTense)
			) break;

		if  (     (SynRel == "����") 
			||  (SynRel == "��")
			) break;
	};

	if (l == P.m_GramCorteges.size()) return false;

	return CreateClauseRelation(ClauseRuleNo, ClauseNo1, ClauseNo2, V,  FindFirstNode(ClauseNo2), *Conj);
}


/*
���� � ������� ������ ��������������� ����� ������� ����� ����� ��� (� ������� ����), 
����������� ��������� ����������� ��������� ������� ������, �� ���� ���������� ���� ���, 
� �����������, ������� �� ��������, ���� ��������� �������� ����������� ������, 
�  ����� ��������� ����� �������� ���������,  ���������� � ������ �����. 
��������: � ����� � ���, ��� �� ����

*/
bool CRusSemStructure::TryClauseCHTO_TOT_Dop(long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{

	const CDictUnitInterp* Conj =   GetSubordConjFromTheBegining(ClauseNo2);
	if (Conj == 0) return false;

	// ��������, ��� ��� "���" 
	long NodeNo = FindFirstNode(ClauseNo2);
	if  (NodeNo == -1) return 0;
	if (!m_Nodes[NodeNo].IsLemma("���")) return false;

	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	if (Nodes2.size() != 2)  return false;

	// ���� ��� ��������� �����
	long i= m_Clauses[ClauseNo1].m_EndNodeNo - 1;

	for (; i >= m_Clauses[ClauseNo1].m_BeginNodeNo; i++)
		if (m_Nodes[i].IsWordContainer())
			break;

	if (    !m_Nodes[i].IsLemma("���")
		|| !m_Nodes[i].HasOneGrammem (rNeutrum)
		)
		return false;

	vector<long> Rels;
	GetIncomingInClauseRelations(i,Rels);
	if (Rels.size() != 1) return false;
	CValency V = m_Relations[Rels[0]].m_Valency;
	if (   (m_Relations[Rels[0]].m_Valency.m_RelationStr != "CONTEN")	  // "� ���� ��, ��� �� ����"
		&& (m_Relations[Rels[0]].m_Valency.m_RelationStr != "THEME")	  // "� ���� � ���, ��� �� ����"
		&& (m_Relations[Rels[0]].m_Valency.m_RelationStr != "OBJ")	  // "� ���� ��, ��� �� ����"
		)
		return false;
	/*
	���������� ������� �������  ��� ����, ����� ��� ������� �� �������� �� ����� 
	"�� ������ ����� ���� �� ����, ��� ��������"
	"������� �� ����-�� "  ����� ����� ������������, �� ������� �� ����� THEME ��� CONTEN.  */
	m_Nodes[i].m_bToDelete = true;

	return CreateClauseRelation(ClauseRuleNo, ClauseNo1, ClauseNo2, V,  NodeNo, *Conj);
}


bool CRusSemStructure::TryClauseParenthesis(long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	long i=0;
	for (; i < m_SynClauseRels.size();  i++)
		if (   (m_SynClauseRels[i].m_SynRelName == "�����_������")
			&& (m_SynClauseRels[i].m_SourceNodeNo == ClauseNo1)
			&& (m_SynClauseRels[i].m_TargetNodeNo == ClauseNo2)
			)
			break;

	if (i == m_SynClauseRels.size()) return false;


	vector<long> Nodes1;
	GetRootsWithoutDeletedOrSyntaxTop(*this, ClauseNo1,Nodes1);
	if (Nodes1.size() != 1)  return false;

	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	if (Nodes2.size() != 1)  return false;

	AddRelation(CRusSemRelation(CValency ("����",A_C), Nodes1[0], Nodes2[0],   "�����"));
	m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

	/*
	��� ���� ����� ��������� �������� ������� FindSemFetAgree,
	����� ��������� m_bSaveIsParenthesis
	*/
	m_Nodes[Nodes2[0]].m_bSaveIsParenthesis = true;

	return true;
};

/*
  ��������� ������������ ������ ���� ���
  ��������: "������������� ������������� ��� ���� �������"
*/

bool CRusSemStructure::TryClauseNSO(long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	long i=0;
	for (; i < m_SynClauseRels.size();  i++)
		if (   (m_SynClauseRels[i].m_SynRelName == "����_���")
			&& (m_SynClauseRels[i].m_SourceNodeNo == ClauseNo1)
			&& (m_SynClauseRels[i].m_TargetNodeNo == ClauseNo2)
			)
			break;

	if (i == m_SynClauseRels.size()) return false;

	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	if (Nodes2.size() != 1)  return false;


	/*
	���� ������ ��������������� ������ � ������ 1, ������������� � �������� ��� 
	*/
	long NodeNo = m_Clauses[ClauseNo1].m_BeginNodeNo;
	for (; NodeNo < m_Clauses[ClauseNo1].m_EndNodeNo; NodeNo++)
		if ( !m_Nodes[NodeNo].IsAbstract() )
			if ( m_Nodes[Nodes2[0]].GetMinWordNo() < m_Nodes[NodeNo].GetMinWordNo())
				if (FullGleiche(NodeNo, Nodes2[0]))
					break;
	if ( NodeNo == m_Clauses[ClauseNo1].m_EndNodeNo )	return false;


	AddRelation(CRusSemRelation(CValency ("PROPERT",A_C), NodeNo, Nodes2[0],   "���")); 
	m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

	return true;
};

const CConj SimpleCoordConj[]  =
{
	{"�","" ,"AND"},
	{"�","" , "BUT"},
	{"�����","" , "ALIKE"},	
	{"��","" , "BUT"},
	{"���", "" ,"OR"}
};	


long CRusSemStructure::GetSimpleConj (long NodeNo, const CConj* Conj, long ConjCount) const
{
	if  (NodeNo == -1) return -1;
	if (!m_Nodes[NodeNo].IsPrimitive()) return -1;

	size_t i =0;
	for (; i < ConjCount; i++)
		if (m_Nodes[NodeNo].m_Words[0].m_Lemma == Conj[i].conj)
			break;
	if (i == ConjCount) return -1;
	return i;
};

bool CRusSemStructure::IsCoordConj(long NodeNo)  const
{
	return   (       (m_Nodes[NodeNo].GetType() == OborRoss)
				&& m_pData->Oborottos[m_Nodes[NodeNo].GetUnitNo()].m_bRusCoordConj
		)
		||(      m_Nodes[NodeNo].IsPrimitive()
		&& (m_Nodes[NodeNo].m_Words[0].m_Lemma == "�����")
		)
		||(      m_Nodes[NodeNo].IsPrimitive()
			&&	m_Nodes[NodeNo].HasPOS(CONJ)
			&& (GetSimpleConj(NodeNo,   SimpleCoordConj, (sizeof(SimpleCoordConj) / sizeof(CConj))) != -1)
			);
};

const CDictUnitInterp* CRusSemStructure::GetSubordConjFromTheBegining (long ClauseNo) const
{
	long NodeNo = FindFirstNode(ClauseNo);
	if  (NodeNo == -1) return 0;
	const CDictUnitInterp* Conj =   m_Nodes[NodeNo].GetInterp();

	// ����������� ������ ���� "���� ���� ����������� ���, �� ���� ��� ������" 
	// ��������� ����� ����� ������ ����� ����� ��������
	if  (   (Conj == 0)
		|| (Conj->m_DictType != OborRoss)
		|| !m_pData->Oborottos[Conj->m_UnitNo].m_bRusSubConj 
		|| (NodeNo == m_Nodes.size() - 1)
		)
	{
		vector<long> Rels;
		GetIncomingInClauseRelations(NodeNo,Rels);
		if ( (Rels.size() != 1) || !IsRusSubj(Rels[0])) return 0;
		const CDictUnitInterp* Conj =   m_Nodes[NodeNo+1].GetInterp();

		if (     (Conj != 0)
			&&  (Conj->m_DictType == OborRoss)
			&&  m_pData->Oborottos[Conj->m_UnitNo].m_bRusSubConj
			&&  m_pData->Oborottos[Conj->m_UnitNo].m_bRusSubConjCanBeAfterSubject 
		 )
		 return Conj;
		else
			return 0;
	};

	return Conj;
};

long CRusSemStructure::GetCoordSimpleConj (long m_ClauseNo) const
{
  return GetSimpleConj(FindFirstNode(m_ClauseNo),   SimpleCoordConj, (sizeof(SimpleCoordConj) / sizeof(CConj)));
};


// ========= ����        ====
//����� �� ������, � ����.
//� ������, ��� �� ���.

// == ��������������� ����������� (���������� � ���������)
// ���, ��� � ����, ������
// ����, ����� ������ ��������, �������.

// ==== ������� ����� � ���������������� �������������
//� ���� �����, �����  �� ������.
//� ����� ����, ���� �� �������.
//� ���� ��, ��� �� ������.
//� ��� ���, ��� �� ���.
//� ��� ������ �����, ����� �� �������
//� �������� ���� � ��, ��� � �����
//� ����� � ���, ������ �� ��� ������


// ====== ������� ����� � ������������  GFi = ����_��
// ������, ��� ����, �� ������.
// ��������, ���� ���� 

bool CRusSemStructure::IsConjWord (long NodeNo)  const
{
  return HasGramFetAfterColon (NodeNo, "����");
};

long CRusSemStructure::GetConjWordFromClause (long ClauseNo) const
{
  for (long j=m_Clauses[ClauseNo].m_BeginNodeNo; (j < m_Clauses[ClauseNo].m_EndNodeNo) && ((j - m_Clauses[ClauseNo].m_BeginNodeNo) < ConjWordDistance); j++)
    if (     (m_Nodes[j].GetInterp() != 0) 
		  && m_Nodes[j].IsPrimitive()
		  && m_Nodes[j].m_bConjWord
	   )
		return j;

  return -1;
};

bool CRusSemStructure::TryClauseConjWord (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	long ConjNodeNo = GetConjWordFromClause(ClauseNo2);
	if (ConjNodeNo == -1) return false;
	CDictUnitInterp Conj =  *m_Nodes[ConjNodeNo].GetInterp();

	// � �������� ����� ������ �����
	if (Conj.m_DictType == OborRoss) return false;

	CValency V = GetSemRelOfPrepOrConj(Conj);
	typedef pair<int, string> PosLemmaPair;

	vector<PosLemmaPair> Relats;
	for (long l=0; l < m_Nodes[ConjNodeNo].m_LexFunctFields.size(); l++)
		if (m_Nodes[ConjNodeNo].m_LexFunctFields[l].m_LexFunct == "Relat")
		{
			string Lemma = m_Nodes[ConjNodeNo].m_LexFunctFields[l].m_Lemma;
			int i = Lemma.find ("(");
			DWORD Poses = 0;
			if (i != -1)
			{
				string q = Lemma.substr(i+1,Lemma.find (")")-i-1);
				Lemma.erase(i);
				Trim(Lemma);
				QWORD dummy;
				m_pData->GetCustomGrammems(q, dummy, Poses);
			};

			Relats.push_back(PosLemmaPair(Poses,Lemma));
		};

	// ���� �������  ����������, ��������: ��� '����' ���� '����'
	// � ��� ���,  ��� �� ���.
	long MinDistance = 1000;
	long NodeNo  = -1;

	for (long i=0; i < m_Nodes.size(); i++)
		if (   
			IsInClause(i, ClauseNo1)
			&& m_Nodes[i].IsWordContainer()
			&& (m_Nodes[i].m_MainWordNo != -1)
		 )
		{
			long j=0;
			for (; j < Relats.size(); j++)
				if (   ( m_Nodes[i].m_Words[m_Nodes[i].m_MainWordNo].m_Lemma == Relats[j].second)
					&& (   ( Relats[j].first == 0 )
					|| (m_Nodes[i].m_Words[m_Nodes[i].m_MainWordNo].m_Poses & Relats[j].first) 
					)
					)
					break;

			if (j==Relats.size()) continue;

			long Distance =  m_Nodes[ConjNodeNo].GetMinWordNo() - m_Nodes[i].GetMinWordNo();

			if (    (Distance >  0) 
				&& (Distance < MinDistance)
				)
			{
				MinDistance  =  Distance;
				NodeNo = i;
			};
		};

	if (NodeNo != -1)
		if (GetIncomingRelationsCount(ConjNodeNo) == 1)  
		{

			AddRelation(CRusSemRelation(CValency("THESAME", A_C, GetRossHolder(Ross)), ConjNodeNo, NodeNo,   ""));		 m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

			m_LexFuncts.push_back (CLexFunctRel (ConjNodeNo, NodeNo, "Relat"));
			return true;
			//};
		};


	CSemPattern P;
	CSynRealization SynReal;

	// ��� ����� "���" ����  ��������� �������:TryClauseAnaphoricSubordWithoutAntecedent
	if (m_Nodes[ConjNodeNo].m_Words[0].m_Lemma == "���")
		return false;


	// ������, ��� ����, �� ������.
	for (long i=0; i <m_Nodes.size(); i++)
		if (    IsInClause(i, ClauseNo1)    
			&& (    GetFreeActantPattern (i, P,  SynReal, "����_��", true,  "", true)
			|| GetFreeActantPattern (i, P,  SynReal, "����_��_���_���", true,  "", true)
			)

			)
		{
			long Distance =  m_Nodes[ConjNodeNo].GetMinWordNo() - m_Nodes[i].GetMinWordNo();
			if (    (Distance >  0) 
				&& (Distance < MinDistance)
				)
			{
				MinDistance  =  Distance;
				NodeNo = i;
			};

		};
	if (NodeNo != -1)
	{
		if (!GetFreeActantPattern (NodeNo, P,  SynReal, "����_��",true, "", true))
			GetFreeActantPattern (NodeNo, P,  SynReal, "����_��_���_���",true, "", true);

		P.m_PatternValency.m_RossHolder = GetRossHolder(Ross);
		AddRelation(CRusSemRelation(P.m_PatternValency, NodeNo, ConjNodeNo,   ""));
		m_Relations[m_Relations.size() - 1].m_SynReal = SynReal;
		m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

		/*
		� �������� ����� ���� �����������, ������� ��������� ������ "� ���", ���� 
		������� ����� ������� ��, ��������
		"� ����� � ���, ������ �� ��� ������"
		*/
		WORD O_UnitNo = GetRoss(OborRoss)->LocateUnit("�+�",1);
		vector<long> Nodes;
		GetOutcomingNodes(NodeNo,Nodes, false);
		for (long j=0; j < Nodes.size(); j++)
			if (   m_Nodes[Nodes[j]].IsLemma("���")
				&& m_Nodes[Nodes[j]].HasThePrep(O_UnitNo)
				)
				m_Nodes[Nodes[j]].m_bToDelete = true;




		return true;
	};


	// �� ������ � ����, ����� �� �����.
	// � ����� � ���, ��� �� ����
	if  (   IsLocSemRel(V.m_RelationStr)
		|| (V.m_RelationStr == "TIME")	  )
	{
		long NodeNo = IsLocSemRel(V.m_RelationStr) ?
			FindLocHost(ConjNodeNo,ClauseNo1)
			:FindTimeHost(ConjNodeNo,ClauseNo1);

		if (NodeNo != -1)
		{
			V.m_RossHolder = GetRossHolder(Ross);
			AddRelation(CRusSemRelation(V, NodeNo, ConjNodeNo,   "" ));
			m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

			return true;
		};
		if (IsLocSemRel(V.m_RelationStr) ) return false;

	};


	return false;
}


  // � �����,  ����� �� ����.
  // �� ����� �������, ��� � ����"
  // �� ������� �������, ����� ������

bool CRusSemStructure::TryClauseCHTOBY_GG (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
  long ConjNodeNo = FindFirstNode(ClauseNo2);
  if (ConjNodeNo ==-1) return false;
  
  
  const CDictUnitInterp* Conj =  m_Nodes[ConjNodeNo].GetInterp();
  if (Conj == 0) Conj = &EmptyInterp;
  string ConjStr =  m_Nodes[ConjNodeNo].m_Words[0].m_Word;

  // ���� ���� �������� ����������� � ����.����, �� ��� �� ����, � ������� �����
  if (ConjStr == "���") 
    if (GetIncomingRelationsCount (ConjNodeNo ) > 0) 
		return false;

  // ���� '���' ������ ������ � ���������
  if (ConjStr == "���") 
    if (m_Nodes[ConjNodeNo].GetType() != OborRoss) 
		return false;


  
  if (    (ConjStr == "�����")
	   || (ConjStr == "���") 
                             //  ��������� ������� , ����� �������� , ��� ��� ������������
	 )
  // ���� ������������� �����������, ������� ���������� �����+��_���
  {
      vector<long> Nodes2;
      GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	  if (Nodes2.size() != 2) return false;
	  long RootNodeNo2 = Nodes2[Nodes2.size()-1];

	  EngRusMakeLower(ConjStr);
	  string SynFet1 = string(ConjStr)+string("+��");
	  string SynFet2 = string(ConjStr)+string("+���");
	  string SynFet3 = string(ConjStr)+string("+��_���");
      long MinDistance = 1000;
      long NodeNo  = -1;
	  CValency V;
  	  
      CSemPattern P;
      CSynRealization SynReal;
	  CSynRealization BestSynReal;


      for (long i=0; i < m_Nodes.size(); i++)
	  if (   (m_Nodes[i].m_Words.size()  > 0 )
		  && IsInClause(i, ClauseNo1)
		  && m_Nodes[i].IsWordContainer()
		  && (   (    GetFreeActantPattern (i, P,  SynReal, "", false, SynFet1, true)
		           && !HasRichPOS(Nodes2[1], INFINITIVE) 
		         )
		      || (    GetFreeActantPattern (i, P,  SynReal, "", false, SynFet2, true)
			       && HasRichPOS(Nodes2[1], INFINITIVE)
			     )
			  || (    GetFreeActantPattern (i, P,  SynReal, "", false, SynFet3, true)
			       && m_Nodes[RootNodeNo2].HasOneGrammem (rPastTense)
			     )
		     )
		 )
	  {
	   long Distance =  m_Nodes[ConjNodeNo].GetMinWordNo() - m_Nodes[i].GetMinWordNo();
	   if (    (Distance >  0) 
		    && (Distance < MinDistance)
		   )
	   {
		   BestSynReal = SynReal;
		   MinDistance  =  Distance;
		   NodeNo = i;
		   V = P.m_PatternValency;

	   };
	  };

     if (NodeNo  != -1)
	 {
	   V.m_RossHolder = GetRossHolder(Ross);
       if (! CreateClauseRelation(ClauseRuleNo, ClauseNo1, ClauseNo2, V, ConjNodeNo, *Conj, NodeNo)) return false;
	   BestSynReal.m_Conj = *Conj;
	   m_Relations[m_Relations.size() - 1].m_SynReal =  BestSynReal;
	   m_Relations[m_Relations.size() - 1].m_SyntacticRelation = "����_��";

	   return true;
	 };

  };
  return false;
};


// ������ � ��������, ���������� �����������, �� ����� ����
// ������� � �������, ������� ������� �������� �������� � ������ �����. 


long  CRusSemStructure::GetClauseCoordSimpleConj (long ClauseNo) const
{
  long ConjNodeNo = FindFirstNode(ClauseNo);
  if (ConjNodeNo == -1) return -1;
  const CDictUnitInterp* Conj =  m_Nodes[ConjNodeNo].GetInterp();
  if (Conj == 0)  return -1;
  if (Conj->m_DictType != OborRoss) return -1;
  if (!m_pData->Oborottos[Conj->m_UnitNo].m_bRusCoordConj) return -1;
  return ConjNodeNo;
};

// ������ �������� ����� � ���� �� ���������� � ����������� (���� ��������� ����)
QWORD CRusSemStructure::GetPersonAndNumberGrammemsOfPredicate (long NodeNo)
{
	long Subj = GetRusSubj(NodeNo);
	QWORD Grammems = m_Nodes[NodeNo].GetGrammems() & rAllNumbers;
	/*
	 ����� ���� �� �����������, ���� ���������� ����
	*/
	if (Subj == -1)
		if ( m_Nodes[NodeNo].HasOneGrammem (rPastTense) )
			Grammems |= rAllPersons;
		else
			Grammems |= (m_Nodes[NodeNo].GetGrammems() & rAllPersons);
	else
		if ( m_Nodes[Subj].HasGrammems (rAllPersons) )
			Grammems |= (m_Nodes[Subj].GetGrammems() & rAllPersons);
		else
			Grammems |= _QM(rThirdPerson);

	return Grammems;	
};


// ���� ������, � ����� ������
bool CRusSemStructure::TryClauseCoordSimpleConj (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
  // "��� �� ���� �������, � ���� � �� ��������� �����, ��� ��������� ���� �������"
  // ����� �������������� ����� ����� ������ � ������ �������
  vector<long> Nodes1;
  GetRootsWithoutDeletedOrSyntaxTop(*this, ClauseNo1,Nodes1);
  if (Nodes1.size() == 0) return false;
  
  vector<long> Nodes2;
  GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
  if (Nodes2.size() < 2) return false;

/*  if (m_Clauses[ClauseNo2].m_bHasParticleConjAnd)
  {
	  CDictUnitInterp Conj;
	  Conj.m_DictType  = OborRoss;
	  Conj.m_UnitNo  = GetRossHolder(OborRoss)->LocateUnit ("�", 1);
	  CValency V = GetSemRelOfPrepOrConj(Conj);
	  if (Nodes1.size() != 1) return false;
      AddRelation(CRusSemRelation(V, Nodes1[0], Nodes2[Nodes2.size() - 1],   ""));
	  m_Relations[m_Relations.size()-1].m_SynReal.m_Conj = Conj;
	  m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

	  return true;
  };*/

  long ConjNodeNo = GetClauseCoordSimpleConj(ClauseNo2);
  if (ConjNodeNo == -1) return false;
  const CDictUnitInterp* Conj =  m_Nodes[ConjNodeNo].GetInterp();
  CValency V = GetSemRelOfPrepOrConj(*Conj);

  long PredicNodeNo1 = Nodes1[Nodes1.size() - 1];
  long PredicNodeNo2 = Nodes2[Nodes2.size() - 1];
  
  if (m_Nodes[PredicNodeNo1].HasPOS(ADVERB_PARTICIPLE) != m_Nodes[PredicNodeNo2].HasPOS(ADVERB_PARTICIPLE) ) return false;
  if (m_Nodes[PredicNodeNo1].HasPOS(PARTICIPLE) != m_Nodes[PredicNodeNo2].HasPOS(PARTICIPLE) ) return false;
  /*
   ���� � �����  ��  ����� ��� �����������, � ��� ��� ������  ����, ����� ����� ��������� 
   ������������ ����� ��������� �����
  */
  
  if (   !m_Nodes[PredicNodeNo2].HasPOS(ADVERB_PARTICIPLE)
	  && !m_Nodes[PredicNodeNo2].HasPOS(PARTICIPLE)
	 )
  {
	  long Subj1 = GetRusSubj(PredicNodeNo1);
	  long Subj2 = GetRusSubj(PredicNodeNo2);
	  if ( (Subj1 == -1) || (Subj2 == -1) )
	  {
		  QWORD Grams =   (GetPersonAndNumberGrammemsOfPredicate (PredicNodeNo1)
					  &  GetPersonAndNumberGrammemsOfPredicate (PredicNodeNo2));

		  if (   ((Grams & rAllPersons) == 0)
			  || ((Grams & rAllNumbers) == 0)
			 )
			 return false;
	  };
  };


  AddRelation(CRusSemRelation(V, PredicNodeNo1, PredicNodeNo2,  "" ));
  m_Relations[m_Relations.size()-1].m_SynReal.m_Conj = *Conj;
  m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;
  m_Nodes[Nodes2[0]].m_bToDelete = true;
  return true;
}




bool CRusSemStructure :: GetFreeActantPattern (long NodeNo, CSemPattern& P, CSynRealization& SynReal, string SynRel, bool CheckSynRel,  string SynFet, bool CheckSynFet)
{

	SynReal.SetEmpty();
	vector<CValency> ValencyMisses;

	GetValencyMisses (NodeNo, ValencyMisses);

	for (BYTE  ValencyNo = 0; ValencyNo < m_Nodes[NodeNo].m_Vals.size(); ValencyNo++)
	{
		GetActantPattern (NodeNo, ValencyNo, P);

		if (     _find (ValencyMisses, P.m_PatternValency) )
		{

			long CortegeNo=0;
			for (; CortegeNo <P.m_GramCorteges.size(); CortegeNo++)
				if  (     CheckSynRel
					&&  !CheckSynFet
					&&  (P.GetSynRelStr(CortegeNo) == SynRel)
					) 
					break;
				else
					if  (      CheckSynFet
						&&  !CheckSynRel
						&&  (P.GetSynFetStr(CortegeNo) == SynFet)
						) 
						break;
					else
						if  (    CheckSynFet
							&& CheckSynRel
							&&  (P.GetSynFetStr(CortegeNo) == SynFet)
							&&  (P.GetSynRelStr(CortegeNo) == SynRel)
							) 
							break;


			if (  CortegeNo <  P.m_GramCorteges.size())
		 {
			 SynReal.m_Cortege = P.m_GramCorteges[CortegeNo];
			 SynReal.m_CortegeNo = CortegeNo;
			 return true;
		 };

		};
	};
	return false;		
};

bool CRusSemStructure::TryClauseAnaphoricSubordWithoutAntecedent (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
// � ������, ��� �� ������
// �� �����, ��� ��� ��������� ������

  vector<long> Nodes1;
  GetClauseRootsWithoutDeleted(ClauseNo1,Nodes1);
  if (Nodes1.size() != 1)  return false;

  long  AnaphoricNodeNo = FindFirstNode (ClauseNo2);
  if (AnaphoricNodeNo == -1) return false;
  if (!m_Nodes[AnaphoricNodeNo].IsPrimitive()) return false;
  if (    (m_Nodes[AnaphoricNodeNo].m_Words[0].m_Lemma != "���") 
	  &&  (m_Nodes[AnaphoricNodeNo].m_Words[0].m_Lemma != "���") 
	  &&  (m_Nodes[AnaphoricNodeNo].m_Words[0].m_Lemma != "���") 
	 )
	 return false;


  vector<long> Nodes2;
  GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
  if (   (Nodes2.size() != 1)
	  && (     (Nodes2.size() != 2)
	        || (GetIncomingRelationsCount(AnaphoricNodeNo) > 0)  
	     )  
	 )
  return false;


  long NodeNo = Nodes1[0];
  while  (NodeNo != -1) 
  {
	   CSemPattern P;
	   CSynRealization SynReal;
	   
	   if  (    (Nodes2.size() == 1) // ������� ����� (�������� ������ ����������� � ����. ������)
			&& (!GetFreeActantPattern(NodeNo, P, SynReal, "����_��_���_���",true, "", false))
			&& (    GetFreeActantPattern(NodeNo, P, SynReal, "�_���", true, "", false)
			    ||  GetFreeActantPattern(NodeNo, P, SynReal, "����_��",true, "", true)
			   )
			   /* GF  = �_���:�
				  GF  = ����_��
				  GF  != ����_��_���_���
			   */
			
		   )
	   {
              // � ������, ��� �� ������
		      // � ����, ��� �� �����
			  P.m_PatternValency.m_RossHolder = GetRossHolder(Ross);
		      AddRelation(CRusSemRelation(P.m_PatternValency, NodeNo, AnaphoricNodeNo,   ""));
			  m_Relations[m_Relations.size()-1].m_SynReal =  SynReal;
			   m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

		      return true;
	   }
	   else // ����
		 if (	     (m_Nodes[AnaphoricNodeNo].GetType() == OborRoss)
				 &&  m_Nodes[AnaphoricNodeNo].IsPrimitive()
				 &&  (m_Nodes[AnaphoricNodeNo].m_Words[0].m_Lemma == "���")
			)
	     if (        GetFreeActantPattern(NodeNo, P,SynReal, "",false, "���+��", true)
			     &&  (Nodes2.size() == 2)
			     && !HasRichPOS(Nodes2[1], INFINITIVE) // "� ����, ��� ������", ��������� �� ����� ����������� ����� "���"
			) 
		  {
				 // � ����, ��� �� ����
			     // � ������, ��� �� ����
 				 m_Nodes[AnaphoricNodeNo].m_bToDelete = true;
				 GetClauseRootsWithoutDeleted(ClauseNo2, Nodes2);
				 assert (Nodes2.size() == 1);
				 P.m_PatternValency.m_RossHolder = GetRossHolder(Ross);
				 AddRelation(CRusSemRelation(P.m_PatternValency, NodeNo, Nodes2[0],   ""));
				 m_Relations[m_Relations.size()-1].m_SynReal =  SynReal;
				 m_Relations[m_Relations.size() - 1].m_SynReal.m_Conj = *m_Nodes[AnaphoricNodeNo].GetInterp();
				 m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

				 return true;

		 };
	    
       long Value = 0;
	   long BestNodeNo = -1;
       for (long i=0; i<m_Nodes.size(); i++)
		if (   IsInClause(i, ClauseNo1)
			&& (m_Nodes[i].IsWordContainer())
			&& m_Nodes[NodeNo].IsWordContainer()
			&& (m_Nodes[NodeNo].GetMinWordNo() < m_Nodes[i].GetMinWordNo())
			&& (FindFirstRelation(NodeNo,i) != -1)
			&& (Value < (m_Nodes[i].GetMinWordNo() - m_Nodes[NodeNo].GetMinWordNo()))
		   )
		{
			Value = m_Nodes[i].GetMinWordNo() - m_Nodes[NodeNo].GetMinWordNo();
			BestNodeNo = i;
		};

		NodeNo = BestNodeNo;

	 };

     if  (    ( Nodes2.size() == 1) // ������� ����� (�������� ������ ����������� � ����. ������)
		   && (m_Nodes[AnaphoricNodeNo].GetType() != OborRoss)
		   && m_Nodes[AnaphoricNodeNo].IsPrimitive()
		   && (m_Nodes[AnaphoricNodeNo].m_Words[0].m_Lemma == "���")
		 )
	   {
              
		    /*
		      ��� �������� �������� ����� "���" ������������ �������� �����,
              ��������, �� ������
			  "�� ��������, ��� ��������� ��������������" ���  
			  "������� ���������� �� ������� ���������� , ��� ���������� ����������� ���������� ����"
               ������� �������, ��� ��� ��������� �������������,
			   � �������, ��� �����-������ ������ ������ ���������
		   */

				CValency V ("CONTEN", A_C);			  
				V.m_RossHolder = GetRossHolder(Ross);
				V.m_bOptional = true;
				AddRelation(CRusSemRelation(V, Nodes1[0], AnaphoricNodeNo,   ""));
				m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;
				return true;
	   }
	   

   return false;
 
};

/*
1)  ���������, ��� ������ ����������� ��������� ������ SF1 = ANIM ��� SF1 = ORG, ���� ���, �� ����� �� ���������;2)  ���������, ���� �� ������������ ����� ���������� ������ ������,  ���� ��� ����;
3)  �������� ����� ���� ����� "���" � �������� ������;
4)  ����� ������������� ����������� �� ���������� ������ ��� ������������ ��� �� ����������������:
������� ����, ��� ����� ������� �� ��� => ������� ����, ��� �� ����� ������� �� ���
������� �����, ��� ����� ������� �� ��� => ������� �����, ��� ��� ����� ������� �� ���
�������� �����, ��� ����� ������� �� ��� => �������� �����, ��� ��� ����� ������� �� ���
5) ��������� ����� ����������� �� ����� �����������
8) �������� ������� ���� �� ������� ������ ������ � ����. ����;
9) �������� ������� THESAME �� ����. ���� � ����������� ������ ������;
*/

CRusSemNode  CRusSemStructure::CreatePronounByLemma(string Lemma)
{
	const CLemmatizer* P = m_pData->GetRusLemmatizer(); 
	vector<CFormInfo> ParadigmCollection;
	P->CreateParadigmCollection(true, Lemma,false, false, ParadigmCollection);
	assert (!ParadigmCollection.empty());
	string GramCodes = ParadigmCollection[0].GetSrcAncode();
	QWORD Grammems = m_pData->GetRusGramTab()->GetAllGrammems(GramCodes.c_str());


	CRusSemWord SemWord(-1, Lemma);
	SemWord.SetFormGrammems( Grammems );
	SemWord.m_Poses = 1 << PRONOUN;
	SemWord.m_Word = Lemma;
	EngRusMakeLower(SemWord.m_Word);
	SemWord.m_CharCase = LowLow;
	SemWord.m_ParadigmId = ParadigmCollection[0].GetParadigmId();
	SemWord.m_IsPunct =  false;
	SemWord.m_pData =  m_pData;
	CRusSemNode N;
	N.m_Words.push_back(SemWord);
	N.SetGrammems ( SemWord.GetAllGrammems() );
	N.m_GramCodes = GramCodes;
	N.m_SynGroupTypeStr = "";
	N.SetMainWordNo(0);
	EngRusMakeLower(Lemma);
	CDictUnitInterp I;
	I.m_UnitNo = GetRossHolder(Ross)->LocateUnit(Lemma.c_str(), 1);
	I.m_DictType   = Ross;
	assert (I.m_UnitNo != ErrUnitNo);
	N.AddInterp(I);
	return N;
};


CRusSemNode  CRusSemStructure::CreateAnaphoricPronoun (long PrototypeNode)
{
    if (HasRichPOS (m_Nodes[PrototypeNode], PRONOUN))
	 return  m_Nodes[PrototypeNode];

    string Lemma;
	if (m_Nodes[PrototypeNode].HasOneGrammem (rPlural) )
		Lemma = "���";
	else
	  if (m_Nodes[PrototypeNode].HasOneGrammem (rNeutrum))
			Lemma = "���";
	  else
	   if (m_Nodes[PrototypeNode].HasOneGrammem (rMasculinum))
			Lemma = "��";
	    else
		  if (m_Nodes[PrototypeNode].HasOneGrammem (rFeminum))
			Lemma = "���";
		  else
			  if (m_Nodes[PrototypeNode].m_NodeType == MNA)
				  Lemma = "���";
			  else
				  Lemma = "��";

  CRusSemNode N	= CreatePronounByLemma(Lemma);
  N.m_NodeSemFets = m_Nodes[PrototypeNode].m_NodeSemFets;
  N.m_SemCategory = m_Nodes[PrototypeNode].m_SemCategory;
  return N;
};

bool CRusSemStructure::TryClauseCHTO_WITH_ANIMAT (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	return false;
//0)
  long ConjNodeNo = FindFirstNode(ClauseNo2);
  if ( (ConjNodeNo ==-1) || !m_Nodes[ConjNodeNo].IsPrimitive() ) return false;
  
  CDictUnitInterp* Conj =  m_Nodes[ConjNodeNo].GetInterp();
  string ConjStr =  m_Nodes[ConjNodeNo].m_Words[0].m_Word;

  if (ConjStr != "���")   return false;
  if (Conj == 0) return false;
  if (Conj->m_DictType != Ross) return false;

  // ���� ��� ����, �� ������� �� ����� ������� 
  CRelSet R = GetIncomingRelations(ConjNodeNo,  false);
  if (R.m_RelsCount == 0) 
		return false;
  if (m_Relations[R.m_Rels[0]].m_SyntacticRelation != "����")
	  return false;
  long Root2 = m_Relations[R.m_Rels[0]].m_SourceNodeNo;
  // 1)  ���������, ��� ������ ����������� ��������� ������ SF1 = ANIM ��� ������������� ANIM
  // ��������, ORG, ���� ���, �� ����� �� ���������;  
  if (m_Nodes[Root2].GetType() == NoneRoss)
	  return false;

  if (!SemFetActantIsEqualOrLower ( GetRossHolder(m_Nodes[Root2].GetType()),
						 m_Nodes[Root2].GetUnitNo(), 
						 1,
						 0,
						"ANIM",						 
						&m_pData->m_HierarchySemFetDoc ) )
						 return false;

 vector<long> Nodes1;
 GetClauseRootsWithoutDeleted(ClauseNo1, Nodes1);
 if (Nodes1.size() != 1) return false;
 long PrototypeSubj = GetRusSubj(Nodes1[0]);
 if (PrototypeSubj == -1) return false;
 //2)  ���������, ���� �� ������������ ����� ���������� ������ ������,  ���� ��� ����;
 if (!CheckSubjectPredicateRelation (PrototypeSubj, Root2)) return false;
 
 
 CRusSemNode NewSubj = CreateAnaphoricPronoun(PrototypeSubj);
 long WordNo = m_Nodes[ConjNodeNo].m_Words[0].m_WordNo+1;
 FreeWordNo(WordNo);
 NewSubj.m_Words[0].m_WordNo = WordNo;
 NewSubj.m_ClauseNo = ClauseNo2;
 CRusSemRelation newRel = m_Relations[R.m_Rels[0]];
 EraseRelation(R.m_Rels[0]);
 newRel.m_TargetNodeNo =  InsertNode(NewSubj);
 AddRelation(newRel);
 //������ ���������� false, ��������� ������ ��������� ������ ��������� ����������� ����� �� �����
 // "���", ������� ������ ��� ����, ���� �� ����� ������������
 Conj->m_DictType  = OborRoss;
 Conj->m_UnitNo  = GetRossHolder(OborRoss)->LocateUnit ("���", 1);
 return false;
};


bool CRusSemStructure::TryClauseZeroSubordWithoutAntecedent (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	// � �����,  �� ����
	long NodeNo2 = FindFirstNode(ClauseNo2);
	if  (NodeNo2 == -1) return false;
	// ���� ��������� ����� ����
	long NodeNo1 = FindLeftClosestNode(NodeNo2);
	if  (NodeNo1 == -1) return false;
	if (!IsInClause(NodeNo1, ClauseNo1) ) return false;
	// ��������� ������� ������������� �����������  �� 0+��
	CSemPattern P;
	CSynRealization SynReal;
	if (!GetFreeActantPattern(NodeNo1, P, SynReal, "",false, "0+��", true) ) return false;
	P.m_PatternValency.m_RossHolder = GetRossHolder(Ross);

	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	if (   (Nodes2.size() != 1) ) return false;
	AddRelation(CRusSemRelation(P.m_PatternValency, NodeNo1, Nodes2[0],   ""));
	m_Relations[m_Relations.size() - 1].m_SynReal = SynReal;
	m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

	return true;
};

bool CRusSemStructure::TryClauseSimpleComma (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
  if (m_ClauseConnectionType !=  AllPossibleClauseRules)	return false;
// � ������,  �� ����
  long NodeNo2 = FindFirstNode(ClauseNo2);
  if  (NodeNo2 == -1) return false;

  // ���� ���������  �� �����������  ���� ������ 1
  long NodeNo1 = m_Clauses[ClauseNo1].m_EndNodeNo - 1;
  for ( ;(NodeNo1 >=m_Clauses[ClauseNo1].m_BeginNodeNo) &&  m_Nodes[NodeNo1].IsAbstract();NodeNo1--)
  {
	  // no body
  }

 
  if  (NodeNo1 < m_Clauses[ClauseNo1].m_BeginNodeNo) return false;
  
  if (m_Nodes[NodeNo1].GetMinWordNo() > m_Nodes[NodeNo2].GetMinWordNo()) return false;
  if ( !m_Nodes[NodeNo1].HasPostPunct(',') ) return false;
  
  vector<long> Nodes1;
  GetClauseRootsWithoutDeleted(ClauseNo1,Nodes1);
  if (   (Nodes1.size() != 1) ) return false;
  
  vector<long> Nodes2;
  GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
  if (   (Nodes2.size() != 1) ) return false;


  if (m_Nodes[Nodes1[0]].HasPOS(ADVERB_PARTICIPLE) != m_Nodes[Nodes2[0]].HasPOS(ADVERB_PARTICIPLE) ) return false;
  if (m_Nodes[Nodes1[0]].HasPOS(PARTICIPLE) != m_Nodes[Nodes2[0]].HasPOS(PARTICIPLE) ) return false;


  AddRelation(CRusSemRelation(CValency("AND", A_C), Nodes1[0], Nodes2[0],   "������"));
  m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;
  return true;
};


bool CRusSemStructure::TryBracketClause(long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	// � ������ (�� ����)
	if (ClauseNo1+1 != ClauseNo2) return false;
	if (!m_Clauses[ClauseNo2].m_bBracketed) return false;

	vector<long> Nodes1;
	GetClauseRootsWithoutDeleted(ClauseNo1,Nodes1);
	if (   (Nodes1.size() != 1) ) return false;

	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	if (   (Nodes2.size() != 1) ) return false;

	AddRelation(CRusSemRelation(CValency("TOGETHER", A_C), Nodes1[0], Nodes2[0],   "������"));  m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;
	return true;
};

// ���� �� ������, �� � ����
// ��� ����� �� �����, ��� ���� �� ���
// ����� �� ������, �� � ��� ���� (�����)
// ��� �������� ����, �� � �����
bool CRusSemStructure::TryClauseSubordDoubleConj (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{

  vector<long> ConjNodes;
  size_t i =0;
  for (; i < m_pData->m_DisruptConj.size(); i++)
  {
	const CDoubleConj& C = m_pData->m_DisruptConj[i];
	ConjNodes.clear();
	long ItemNo = 0;
    for (long NodeNo = m_Clauses[ClauseNo1].m_BeginNodeNo; NodeNo < m_Clauses[ClauseNo1].m_EndNodeNo; NodeNo++ )
	{
		if (!m_Nodes[NodeNo].IsPrimitive()) break;
		if (ItemNo == C.m_FirstPart.size()) break;
		string Word =  m_Nodes[NodeNo].m_Words[0].m_Word;
		EngRusMakeUpper(Word);
		if (Word != C.m_FirstPart[ItemNo]) break;
		ConjNodes.push_back(NodeNo);
		ItemNo++;
	}
	if (ItemNo != C.m_FirstPart.size()) continue;
	ItemNo = 0;
    for (long NodeNo = m_Clauses[ClauseNo2].m_BeginNodeNo; NodeNo < m_Clauses[ClauseNo2].m_EndNodeNo; NodeNo++ )
	{
		if (!m_Nodes[NodeNo].IsPrimitive()) break;
		if (ItemNo == C.m_SecondPart.size()) break;
		if (m_Nodes[NodeNo].m_Words[0].m_Lemma != C.m_SecondPart[ItemNo]) break;
		ConjNodes.push_back(NodeNo);
		ItemNo++;
	}
	if (ItemNo != C.m_SecondPart.size()) continue;
	break;
  };
  if (i == m_pData->m_DisruptConj.size()) return false;
  long UnitNo  = m_pData->m_DisruptConj[i].m_UnitNo;
  CDictUnitInterp Conj =   CDictUnitInterp(OborRoss, UnitNo);
  CValency V = GetSemRelOfPrepOrConj(Conj);
  for (i=0; i < ConjNodes.size(); i++)
	  m_Nodes[ConjNodes[i]].m_bToDelete = true;

  vector<long> Nodes2;
  GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
  if (Nodes2.size() != 1) return false;
	 
  CSemPattern P;
  P.InitSemPattern(GetRossHolder(OborRoss), UnitNo, 2,0);
  if (!P.LoadGramFromDict()) return false;

  long l=0;
  for (; l < P.m_GramCorteges.size();l++)
  {
	 string SynRel = P.GetSynRelStr(l);
	 if  (    (SynRel == "���") 
		   && IsInfinitiveOrMNAInfinitive(Nodes2[0])
		 ) 
	 break;
	 if  (    (SynRel == "��_���") 
		   && m_Nodes[Nodes2[0]].HasOneGrammem (rPastTense)  
		 ) break;
     if  (    (SynRel == "����_����") 
		   && (!HasRichPOS (Nodes2[0], INFINITIVE)) 
		 ) 
		 break;

 	 if  (     (SynRel == "����") 
		   ||  (SynRel == "��")
		 ) break;
  };

 if (l == P.m_GramCorteges.size()) return false;

 vector<long> Nodes1;
 GetClauseRootsWithoutDeleted(ClauseNo1,Nodes1);
 if (Nodes1.size() != 1) return false;

 AddRelation(CRusSemRelation(V, Nodes2[0], Nodes1[0],  "" ));
 m_Relations[m_Relations.size()-1].m_SynReal.m_Conj = Conj;
 m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

 m_LexFuncts.push_back (CLexFunctRel (
	  m_Relations[m_Relations.size() - 1].m_SourceNodeNo, 
	  m_Relations[m_Relations.size() - 1].m_TargetNodeNo, 
	  "DoubleConj"));
	  
 return true;
}


// � ����, ��������
bool CRusSemStructure::TryClauseDeeprichastie (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	vector<long> Nodes1;
	GetRootsWithoutDeletedOrSyntaxTop(*this, ClauseNo1,Nodes1);
	for (size_t i =0; i <Nodes1.size(); i++)
		if (    HasRichPOS(Nodes1[i], CONJ) 
			|| HasRichPOS(Nodes1[i], ADV) 
			)
		{
			Nodes1.erase(Nodes1.begin() + i);
			i--;
		}

	if (Nodes1.size() != 1)  return false;
	if  (HasRichPOS (Nodes1[0], ADVERB_PARTICIPLE)) return false;	

	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);

	if (Nodes2.size() != 1)  return false;
	if  (!HasRichPOS (Nodes2[0], ADVERB_PARTICIPLE)) return false;

	vector<long> Rels;

	/*
	����� �� ���� �����������, ��������:
	"����� �������� �����, �� ����� �����."
	*/
	long SubjNodeNo = GetRusSubj(Nodes1[0]);
	if (SubjNodeNo != -1) 
	{
		CValency V  ("F-ACT", A_C, 0);	  
		if  ( m_Nodes[Nodes2[0]].m_Vals.size() > 0 )
			V = m_Nodes[Nodes2[0]].m_Vals[0];

		m_DopRelations.push_back(CRusSemRelation(V, Nodes2[0], SubjNodeNo,   "" ));
	};



	CValency V1  ("TOGETHER", A_C, 0);  AddRelation(CRusSemRelation(V1, Nodes1[0], Nodes2[0],   "" ));
	m_Relations.back().m_ClauseRuleNo = ClauseRuleNo;


	return true;

};

/*
 ���� �� ���� �����, ������� ��������� ����� NodeNo1 �  NodeNo2, ������� ����������� ������ NodeNo1.
 ����� � - ���� ����� ����. ���� ���� �� ���� ��������� (�������� ��� ���������) �����������
 � �����, ������� ��������� �� ���������  ������� [m_Nodes[NodeNo1].GetMinWordNo(), m_Nodes[NodeNo2].GetMinWordNo()],
 �� ������� ���������� ����. � ��������� ������ - ������.
 ����������, ��� ������� - �������� ������������� �������� ������������ ��������� ����� ����� NodeNo
 � ����� NodeNo2.
*/
bool CRusSemStructure::CanHaveRightClauseRelation (long NodeNo1, long NodeNo2) const
{
	long LastClauseNode =  m_Clauses[m_Nodes[NodeNo1].m_ClauseNo].m_EndNodeNo -1;
	long BeginNodeNo  = m_Clauses[m_Nodes[NodeNo1].m_ClauseNo].m_BeginNodeNo;
	for (; LastClauseNode >= BeginNodeNo;  LastClauseNode--)
		if ( m_Nodes[LastClauseNode].IsWordContainer() )
			break;


	for (int i =  NodeNo1+1;  i < LastClauseNode+1; i++)
		if (   m_Nodes[i].IsWordContainer() 
			&& m_Nodes[NodeNo1].IsWordContainer()
			&& m_Nodes[LastClauseNode].IsWordContainer()
			)
			if (m_Nodes[i].GetMinWordNo() < m_Nodes[NodeNo2].GetMinWordNo())
			{
				//  ����� ������������ ��� �������
				if (m_Nodes[i].m_NodeType == Copul)
				{
					vector<long> Rels1;
					GetIncomingInClauseRelations(i, Rels1);
					if (Rels1.size() == 1)
						if (!IsBetween(m_Nodes[m_Relations[Rels1[0]].m_SourceNodeNo], m_Nodes[NodeNo1], m_Nodes[NodeNo2]))
							continue;
				};
				vector<long> Rels;
				GetRelations (i, Rels, false);


				for (long k=0; k < Rels.size(); k++)
				{
					long nd  = (m_Relations[Rels[k]].m_SourceNodeNo == i) ?  m_Relations[Rels[k]].m_TargetNodeNo : m_Relations[Rels[k]].m_SourceNodeNo;
					//  ����� ������������ ��� �������
					if (m_Nodes[nd].m_NodeType == Copul)
					{
						vector<long> Rels1;
						GetIncomingInClauseRelations(i, Rels1);
						if (Rels1.size() == 1)
							nd = m_Relations[Rels1[0]].m_SourceNodeNo;
					};

					if ( ( nd!=NodeNo1) && !IsBetween(m_Nodes[nd], m_Nodes[NodeNo1], m_Nodes[NodeNo2])  )
						return false;
				};
			};
	return true;
};


//������ ���, �������� �� ��������
bool CRusSemStructure::TryClausePrichastie (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
  vector<long> Nodes2;
  GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
  
  if (Nodes2.size() != 1)  return false;
  if  (!HasRichPOS (Nodes2[0], PARTICIPLE)) return false;

  vector<long> Rels;
  GetOutcomingRelations(Nodes2[0], Rels, false);
  

  long i=0;
  for (; i < m_Nodes[Nodes2[0]].m_Vals.size(); i++)
  {
	  long k=0;
	  for (; k < Rels.size(); k++)
	  {
		  assert (i < m_Nodes[Nodes2[0]].m_ValsLayout.size());
		  size_t debug = m_Nodes[Nodes2[0]].m_ValsLayout.size();
		  debug = m_Nodes[Nodes2[0]].m_ValsLayout[i];
		  debug = m_Nodes[Nodes2[0]].m_Vals.size();
		  if (m_Relations[Rels[k]].m_Valency == m_Nodes[Nodes2[0]].m_Vals[m_Nodes[Nodes2[0]].m_ValsLayout[i]])
			break;
	  }

      if ( k == Rels.size() )
		  break;
  };
  if ( i == m_Nodes[Nodes2[0]].m_Vals.size()) return false;

  CValency  V = m_Nodes[Nodes2[0]].m_Vals[m_Nodes[Nodes2[0]].m_ValsLayout[i]];

  long NounNodeNo  = -1;
  if (m_Clauses[ClauseNo2].m_ClauseType  == PARTICIPLE_T)
  {
	for (long i = m_Clauses[ClauseNo1].m_EndNodeNo-1; i >= m_Clauses[ClauseNo1].m_BeginNodeNo; i--)
	  if ( m_Nodes[i].IsWordContainer() && m_Nodes[Nodes2[0]].IsWordContainer() )
	  if (FullGleiche(i, Nodes2[0]))
	  if (    (m_Nodes[i].GetMinWordNo() <  m_Nodes[Nodes2[0]].GetMinWordNo())
		   && CanHaveRightClauseRelation(i, Nodes2[0])
		 )
	  {
		  NounNodeNo = i;
	      break;
	  };
  }
  else
	if (m_Clauses[ClauseNo2].m_ClauseType  == UNDETACHED_ADJ_PATIC)
	{
		for (long i =  m_Clauses[ClauseNo1].m_BeginNodeNo; i < m_Clauses[ClauseNo1].m_EndNodeNo; i++)
			if (m_Nodes[i].IsWordContainer() && m_Nodes[Nodes2[0]].IsWordContainer() )
			if (FullGleiche(i, Nodes2[0]))
		    if (m_Nodes[i].GetMinWordNo() >  m_Nodes[Nodes2[0]].GetMinWordNo())
			  {
				  NounNodeNo = i;
				  break;
			  };
	};
	  
  if (NounNodeNo != -1)
  {
		  /*
		   ����������� ���������,������� ��������� ��������������,  
		   ������� � �������������� ���������,  � �� � �������� ����.
			��� �����, ����� �� �������� ���������� ���������� ������, 
			�.�. ������� ��  ��� �� ���������������� � ���������.
		  */
		  m_DopRelations.push_back(CRusSemRelation(V, Nodes2[0], NounNodeNo,   "" ));
		   
			AddRelation(CRusSemRelation(CValency("PROPERT", A_C), NounNodeNo, Nodes2[0],   "" ));		  
			m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;

		  // ���� ����� ���� ������������� ��������� 
		  if ( m_Nodes[Nodes2[0]].HasOneGrammem (rPassiveVoice) )
			 // ���������  ��������� LF ���� "�������, ����������� �����"
			 // ��� "������, ������������ �����"
			 BuildVerbLexFunctParameterForTheNodes(NounNodeNo, Nodes2[0]);

		  /*
		    ���� ������� ����� �����������, �  ��������� ����� ������ ���� �����,
			����� ����� ������������� ����� �� ������� �����, ��������� 
			���������� � ������� ������ ������ ���� ����������� �� �����
			��������: "������, ��������� ����"
		  */
		  if  ( m_Nodes[NounNodeNo].HasGrammems (rAllNumbers) )
			 if ( (m_Nodes[Nodes2[0]].GetGrammems() & rAllNumbers) == _QM(rSingular))
				 m_Nodes[NounNodeNo].DeleteGrammems ( _QM(rPlural) );
			 else
				 if ( (m_Nodes[Nodes2[0]].GetGrammems() & rAllNumbers) == _QM(rPlural))
					 m_Nodes[NounNodeNo].DeleteGrammems ( _QM(rSingular) );

		  return true;
	};

  return false;
};


long CRusSemStructure::GetKOTORYJorKAKOJ (long ClauseNo) const
{
	for (size_t i=0; i < m_Nodes.size(); i++)
		if (IsInClause(i, ClauseNo))
			if ( m_Nodes[i].IsPrimitive() )
				if  (      (m_Nodes[i].m_Words[0].m_Lemma == "�������")
					|| (m_Nodes[i].m_Words[0].m_Lemma == "�����")
					)
					return i;

	return -1;
};

//�  ���� ��������,  �������� �� �����
// � ����� �������, ����� � ������� �� �����.
bool CRusSemStructure::TryClauseKOTORYJ (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	long KOTORYJ_NodeNo = GetKOTORYJorKAKOJ(ClauseNo2);

	if (KOTORYJ_NodeNo == -1) return false;

	long nd = FindLeftClosestNode(KOTORYJ_NodeNo);
	for (; nd != -1; nd = FindLeftClosestNode(nd))
		if (    IsInClause(nd, ClauseNo1)
			&& HasRichPOS(nd, NOUN)
			&& m_pData->GetRusGramTab()->GleicheGenderNumber(m_Nodes[nd].m_GramCodes.c_str(),m_Nodes[KOTORYJ_NodeNo].m_GramCodes.c_str())
			)
			break;

	if (nd ==  -1) return false;

	long HostNodeNo  =  nd;
	m_Nodes[nd].m_GramCodes =  m_pData->GetRusGramTab()->GleicheAncode1(GenderNumber0, m_Nodes[nd].m_GramCodes.c_str(),m_Nodes[KOTORYJ_NodeNo].m_GramCodes.c_str());
	m_Nodes[nd].SetGrammems(m_pData->GetRusGramTab()->GetAllGrammems(m_Nodes[nd].m_GramCodes.c_str()) & m_Nodes[nd].GetGrammems());
	AddRelation(CRusSemRelation(CValency("THESAME", A_C, GetRossHolder(Ross)),  KOTORYJ_NodeNo, HostNodeNo,  ""));  
	m_Relations.back().m_ClauseRuleNo = ClauseRuleNo;
	// ��������� �������������� � ���� HostNodeNo �� ���� KOTORYJ_NodeNo, ���� � HostNodeNo ��� ����������
	if (		m_Nodes[HostNodeNo].HasOneGrammem(rAnimative) 
			&&	!m_Nodes[HostNodeNo].HasOneGrammem(rNonAnimative)		
		)
	{
		AddSemFet(m_Nodes[KOTORYJ_NodeNo], "ANIM");
	}
	

	return true;
}


// ���� �� � ����, �� �� ����� �� ������
bool CRusSemStructure::TryClauseConditionalImperative (long ClauseRuleNo, long ClauseNo1, long ClauseNo2)
{
	if (!m_Clauses[ClauseNo2].m_HasParticleBY) return false;

	vector<long> Nodes1;
	GetClauseRootsWithoutDeleted(ClauseNo1,Nodes1);
	if (Nodes1.size() != 1)  return false;
	if ( !m_Nodes[Nodes1[0]].HasOneGrammem (rImperative) ) return false;
	long SubjNodeNo = GetRusSubj(Nodes1[0]);
	if (SubjNodeNo == -1) return false;
	if (!CheckDirection(Nodes1[0], SubjNodeNo, ">>")) return false;


	vector<long> Nodes2;
	GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
	if (Nodes2.size() != 1)  return false;

	AddRelation(CRusSemRelation(CValency("COND", A_C, GetRossHolder(Ross)),  Nodes1[0], Nodes2[0],  ""));    
	m_Relations[m_Relations.size()-1].m_ClauseRuleNo = ClauseRuleNo;


	return true;
};





// ������� + �������� �  ������+ �� ������� (��� ����� � ������, �� ������������, �� ���������)
// �������:
// ������, ����� ��� �������  ���� ����� ����, �� ������ ����.
// �������, ������� ����� �� ����, ��� � �����.
// ���, ��� � ���, ��� ����.  
bool IsMotherLandForPustycha(const CRusSemStructure& S, long ClauseNo) 
{
	long ClauseType = S.m_Clauses[ClauseNo].m_ClauseType;

	if (   (ClauseType == ADVERB_PARTICIPLE_T)
		|| (ClauseType == PREDK_T)
		|| (ClauseType == INFINITIVE_T)
		|| (ClauseType == PARTICIPLE_T)
		|| (ClauseType == UnknownSyntaxElement)
	   )
   return false;

   if (S.GetSubordConjFromTheBegining (ClauseNo) != 0) return false;
   if (S.GetKOTORYJorKAKOJ (ClauseNo) != -1) return false;

   return true;
};

struct CMotherLandHyp {
	const CRusSemStructure& m_Parent;
	long m_MainClauseNo; 
	long m_PustychaClauseNo; 
	long m_Distance;

	// ����� ������� �����, ��������, �� � �� ����� ���������� �����.
	long m_bGleicheWithLastNoun;

    // �� ��� ������ ������, ��� �������.
	bool m_bCompAdjAndCHEM;

    // �� ��� �� ��������, � ��������
	bool m_bNegAndSOJUZ_A;

	CMotherLandHyp (const CRusSemStructure& Parent, long MainClauseNo, long PustychaClauseNo,  bool bGleicheWithLastNoun, bool bCompAdjAndCHEM, bool bNegAndSOJUZ_A)
		: m_Parent(Parent)

	{
		
		m_MainClauseNo = MainClauseNo;
		m_PustychaClauseNo = PustychaClauseNo;
		m_bGleicheWithLastNoun = bGleicheWithLastNoun;
		m_bCompAdjAndCHEM = bCompAdjAndCHEM;
		m_bNegAndSOJUZ_A = bNegAndSOJUZ_A;
		m_Distance = 10000;
		for (int i =0; i<Parent.m_Nodes.size(); i++)
			for (int j = i+1; j<Parent.m_Nodes.size(); j++)
				if	(			(Parent.m_Nodes[i].m_ClauseNo == MainClauseNo)
						&&		(Parent.m_Nodes[j].m_ClauseNo == PustychaClauseNo)
					)
				{
					long Dist  = abs(Parent.m_Nodes[i].GetMinWordNo() -  Parent.m_Nodes[j].GetMinWordNo());
					m_Distance  = min (m_Distance , Dist);
				};
	};

	long GetDistance () const 
	{
		return m_Distance;
	};

    long GetWeightOfHypot () const 
	{  
	  return GetDistance() - (m_bGleicheWithLastNoun ? 1 : 0) - (m_bCompAdjAndCHEM ? 2 : 0) - (m_bNegAndSOJUZ_A ? 2 : 0); 
	};

    bool operator < (CMotherLandHyp & X)  const  {return GetWeightOfHypot() < X.GetWeightOfHypot(); };
    bool operator == (CMotherLandHyp & X) const {return GetWeightOfHypot() == X.GetWeightOfHypot(); };
	CMotherLandHyp& operator = (const CMotherLandHyp & X)
	{
		m_MainClauseNo = X.m_MainClauseNo; 
		m_PustychaClauseNo = X.m_PustychaClauseNo; 
		m_Distance = X.m_Distance;
		m_bGleicheWithLastNoun = X.m_bGleicheWithLastNoun;
		m_bCompAdjAndCHEM = X.m_bCompAdjAndCHEM;
		m_bNegAndSOJUZ_A = X.m_bNegAndSOJUZ_A;
		return *this;

	};
};


bool IsSubordConj(string S)
{ 
	return    ( S == "���")
		  ||  ( S == "���")
		  ||  ( S == "�����")
		  ||  ( S == "�����")
		  ||  ( S == "������")
		  ||  ( S == "���")
		  ||  ( S == "�������")
		  ||  ( S == "�����");
};


void FindAllMotherLandHypots(const CRusSemStructure& S, vector<CMotherLandHyp>& MotherLandHyps)
{
	for (long i=0; i < S.m_Clauses.size(); i++)
	if (!S.m_Clauses[i].m_bBracketed)
		if (   (S.m_Clauses[i].m_ClauseType == UnknownSyntaxElement)
		   // "��� �����, ������� � ������  ������" - ��������� �����  ��� ����������� �� ��� ������
		   // ������� ���������� ����������  ��� �����������
		 || (    (S.m_Clauses[i].m_ClauseType == COMPARATIVE_T)
			  && (S.GetCoordSimpleConj(i) != -1)
			)
		)
		{

			// �� ��� ������, ��� �������.
			long NodeNo = S.FindFirstNode(i);
			if (NodeNo == -1) continue;
			bool StartWithCHEM  = S.m_Nodes[NodeNo].IsLemma("���");
			bool StartWithSOJUZ_A  = S.m_Nodes[NodeNo].IsLemma("�");
			bool StartWithSubord =    
								(S.GetSubordConjFromTheBegining(i) !=0)
							|| S.IsConjWord(NodeNo);

			/*
			���� ������, ������� �������� � ���� ������� �������, ���� �����, �����
			������� ��� ������ ���������, �������:
			"���, � ���� �������, ����� ��������� �������� ������"
			����� "� ���� �������" � ��������� �������� �������� ��������, ������� ����� 
			���������� � ���������� ��� ������� ������.
			*/
			bool HasBiggerClause = false;
			if (S.m_Clauses[i].m_ClauseType == UnknownSyntaxElement)
			{
				for (long k=0; k < S.m_Clauses.size();k++)
					if(i != k)
						if (	(S.m_Nodes[S.m_Clauses[k].m_BeginNodeNo].GetMinWordNo() < S.m_Nodes[S.m_Clauses[i].m_BeginNodeNo].GetMinWordNo())
								&& (S.m_Nodes[S.m_Clauses[i].m_EndNodeNo-1].GetMinWordNo() < S.m_Nodes[S.m_Clauses[k].m_EndNodeNo-1].GetMinWordNo())
							)
							{
								HasBiggerClause = true;
								MotherLandHyps.push_back(CMotherLandHyp(S,k,i,false, false, false));
								break;
							};
			};


			if (!HasBiggerClause)
			for (long k=1; ((i-k) >= 0) || ((i+k) < S.m_Clauses.size());k++)
			{
				if ((i-k) >= 0)
					if	   (!StartWithSubord  || StartWithCHEM)
										//  ���� ������ ���������� � ����. �����
									// �� ������ �� ��  ���� ������������
										// ��������, "� ����, ��� ������ � ������� ������ ���� "(1)
										// ���� ��� �������� ���������, �� (1) ������ ����� �������
										// ����� ������ � ������ "���", ��������, "���� ����� ���� ������, ��� ����"
										
						 if (    IsMotherLandForPustycha(S, i-k) 
							 || (     (S.m_Clauses[i].m_ClauseType == COMPARATIVE_T) 
								  &&  S.ClauseHasCompAdj(i-k)
								) 
							)
							MotherLandHyps.push_back(CMotherLandHyp(S, i-k,i,false, StartWithCHEM && S.ClauseHasCompAdj(i-k), StartWithSOJUZ_A && S.ClauseHasNegation(i-k)));

			// "�������, ��� ��������� ����,  ���������"
			// ��� ���� ��������� ����������� ����� "�������" � "����", ������� ����� �� 
			// ������� ������� � "���"  � ������� ������.

			if (S.m_Nodes[NodeNo].IsLemma("���"))
				if ((i-k) >= 0)
				{
				   long NounNodeNo1 = S.FindLeftClosestNode(NodeNo);
				   long NounNodeNo2 = S.GetDefiniteRightHostByNotWeakSynRelation(NodeNo+1);
				   if ((NounNodeNo1 != -1) && (NounNodeNo2 != -1))
				   if ( S.m_pData->GetRusGramTab()->GleicheCaseNumber(S.m_Nodes[NounNodeNo2].m_GramCodes.c_str(),S.m_Nodes[NounNodeNo1].m_GramCodes.c_str()) )
					   MotherLandHyps.push_back(CMotherLandHyp(S,i-k,i,false, false, false));
				};


			if ((i+k) < S.m_Clauses.size())
				if (IsMotherLandForPustycha(S, i+k))
				{
				  NodeNo = S.FindFirstNode(i+k);
				  if (NodeNo == -1) continue;
				  if  (    (S.m_Nodes[NodeNo].IsPrimitive())  
   						&& IsSubordConj ( S.m_Nodes[NodeNo].m_Words[0].m_Lemma) 
					  ) continue;

				  // "��� ��������� , ��� ������ � �������� � ����� , ���� �� ��������"
				  // ����� ����� "��� ���������" ������� � "���� �� ��������"
				  if (S.IsCoordConj ( NodeNo) ) continue;

				  MotherLandHyps.push_back(CMotherLandHyp(S, i+k,i,false, false, false ));
				};
		}
	};


};

// ����������� ��������� ����� �����, ������� �� ���� ���������� �� ����������.
// "�� ��� �������� � ��������" (�� ���������� ��� ������)
// "��� �����, ������� � ������  ������" (�� ���������� ��� ������)
// "�� ��� ������, ��� �������." (�� ���������� ��� ������)

void CRusSemStructure:: FindMotherLandForPustycha()
{ 
	PrintNodes();

	vector<CMotherLandHyp> MotherLandHyps;

	FindAllMotherLandHypots(*this, MotherLandHyps);

	
	vector<VectorLong> Parents;
	Parents.resize(m_Clauses.size());
	for (int i=0; i <MotherLandHyps.size(); i++)
	 Parents[MotherLandHyps[i].m_PustychaClauseNo].push_back(i);

	for (int i=0; i < m_Clauses.size(); i++)
		if (Parents[i].size() == 0)
			Parents[i].push_back(-1);

	VectorLong V; // ������� ������� 
	V.resize(Parents.size());
	vector<VectorLong> Variants; // ��� ��������� ��������
	GetCommonVariants(Parents, V, Variants, 0);
	long BestWeight = 1000;
	long BestVariantNo = -1;
	for (int i=0; i < Variants.size(); i++)
	{
	 long Weight =  0;
	 for (size_t k=0; k < m_Clauses.size(); k++)
	 if (Variants[i][k] != -1)
	 {
		 Weight += MotherLandHyps[Variants[i][k]].GetWeightOfHypot();
		 // �������� ������� ������������� 
		 for (size_t j=0; j < m_Clauses.size();j ++)
		   if (j!=k)
			 if (Variants[i][j] != -1)
			 {
			   long F1 = MotherLandHyps[Variants[i][k]].m_MainClauseNo;
			   long F2 = MotherLandHyps[Variants[i][k]].m_PustychaClauseNo;
			   if (F1 > F2) swap (F1, F2);
			   long G1 = MotherLandHyps[Variants[i][j]].m_MainClauseNo;
			   long G2 = MotherLandHyps[Variants[i][j]].m_PustychaClauseNo;
			   if (G1 > G2) swap (G1, G2);
			   if  (   ( (F1 < G1) && (G1< F2) && (F2 < G2))
					|| ( (G1 < F1) && (F1< G2) && (G2 < F2))
				   )
			   Weight += 2;
		   };

	 };

	 if (Weight < BestWeight)
	 {
		 BestVariantNo = i;
		 BestWeight = Weight;
	 };
	};

	if (BestVariantNo != -1)
	{
		vector<CRusSemNode> SaveNodes;
		SetNodeToDeleteFalse();
		for (long ClauseNo=0; ClauseNo< m_Clauses.size(); ClauseNo++)
		if  (Variants[BestVariantNo][ClauseNo] != -1)
		{
			long MainClauseNo = MotherLandHyps[Variants[BestVariantNo][ClauseNo]].m_MainClauseNo;
			m_Clauses[MainClauseNo].m_AddedClauses.push_back(CSyntaxClauseVariant(m_Clauses[ClauseNo].m_SyntaxClauseNo,m_Clauses[ClauseNo].m_ClauseVariantNo));

			for (long k=0; k < m_Nodes.size(); k++)
			if (IsInClause(k, ClauseNo))
			{
				SaveNodes.push_back(m_Nodes[k]);
				MoveSynRelations(k, -(SaveNodes.size() + 1));
				m_Nodes[k].m_bToDelete = true;
				SaveNodes[SaveNodes.size() - 1].m_ClauseNo = MainClauseNo;
			};
		};
		DelNodesToDelete();
		for (long i=0; i < SaveNodes.size(); i++)
		{
			long nd = InsertNode(SaveNodes[i]);
			MoveSynRelations( -(i + 2), nd);
		};

	}; 

	DeleteEmptyClauses ();
};

/*
 1. ������������ ������������� ����  '�' � ����� ������ �����������. 
 ������� ���, ���� �� ������� ������ ������, ������� ���������� � ����� "�" (
 ����� ���� ���������� �� ������� ����)
 � ������ �����  ��������� ����.

 2. ������������ ������, ����� ���� "�" ����� ����� �������������� ������, 
 ��������,  "���� ����, � ����� �� ������ �����, ��� �������."
*/
void CRusSemStructure:: ProcessParticipleAndInTheFirstClause()
{
	// ������ ������	
	long NodeNo = FindFirstNode(0);
	if (   m_Nodes[NodeNo].IsLemma("��")  && CanBeDeleted(NodeNo))
	{
		m_bHasConjBut = true;
		DelNode(NodeNo); 
		NodeNo = FindFirstNode(0);
		
	};
	if (NodeNo == -1) return;

	if	(		(		m_Nodes[NodeNo].IsLemma("�") 
					|| m_Nodes[NodeNo].IsLemma("�")
				)
			&&	(NodeNo+1 < m_Clauses[0].m_EndNodeNo)
			&&	!m_Nodes[NodeNo+1].IsWordForm(".") // �� ������ ���� � ���������
		)
	{
		// ���� ������� �����
		long i=1;
		for (; i < m_Clauses.size(); i++)
		{
			long NodeNo = FindFirstNode(i);
			if (m_Nodes[NodeNo].IsLemma("�"))
				break;
		};
		// ���� ����� ������� ���� �...�, ������� �� ���������
		if (CanBeDeleted(NodeNo))
			if (		(i == m_Clauses.size() )
					||	(m_Clauses.size() == 1)
				) 
				{
					m_Clauses[0].m_bHasParticleConjAnd = true;
					DelNode(NodeNo);
				}
	}
	// �������� ������	
	for (long ClauseNo = 1; ClauseNo < m_Clauses.size(); ClauseNo++)
	{
		long NodeNo = FindFirstNode(ClauseNo);
		if (NodeNo+1 < m_Clauses[ClauseNo].m_EndNodeNo)
			if (m_Nodes[NodeNo].IsLemma("�") )
				if(!m_Nodes[NodeNo+1].IsWordForm(".") ) // �� ������ ���� � ���������
				{
					const CDictUnitInterp* Conj =   m_Nodes[NodeNo+1].GetInterp();
					if (Conj != 0) 
						if (Conj->m_DictType == OborRoss)  
							if (m_pData->Oborottos[Conj->m_UnitNo].m_bRusSubConj) 
								if (CanBeDeleted(NodeNo))
								{
									m_Clauses[ClauseNo].m_bHasParticleConjAnd = true;
									DelNode(NodeNo);
								};
				};
	};

};

// ��������� �������� �� ���� ������ "�" ���� �� ��� ��  ������� �� ����� �������
// � � ������ ���� ���� ����� ����� �����, ����� ���������, ��� ���� ���� �������
// ��������:
// "� ������ �������������� �� � �������"
void CRusSemStructure:: ProcessIsolatedParticipleAnd()
{
	for (long i =0; i<m_Nodes.size(); i++)
		if (   (m_Nodes[i].IsLemma("�") || m_Nodes[i].IsWordForm(","))
			&& (GetOutcomingRelationsCount(i, false) == 0)
			&& CanBeDeleted(i)
			&& (		(m_Nodes[i].m_ClauseNo == 0)
					|| (i > m_Clauses[m_Nodes[i].m_ClauseNo].m_BeginNodeNo)
				)
			&& i+1 < m_Nodes.size() 
			&& !m_Nodes[i+1].IsWordForm(".") // �� ������ ���� � ���������

		 )
		{
			if (m_Nodes[i].IsLemma("�"))
				m_Clauses[m_Nodes[i].m_ClauseNo].m_bHasParticleConjAnd = true;
			DelNode(i);
			i--;
		};
};

void CRusSemStructure:: DeleteConjInTheBegining(long ClauseNo, CRusSemNode& FirstConjNode)
{
	long NodeNo = FindFirstNode(ClauseNo);
	if  (NodeNo == -1) return ;
	const CDictUnitInterp* Conj =   m_Nodes[NodeNo].GetInterp();
	if (Conj == 0)return;
	if (Conj->m_DictType != OborRoss)  return;
	if (   !m_pData->Oborottos[Conj->m_UnitNo].m_bRusSubConj
		&& !m_pData->Oborottos[Conj->m_UnitNo].m_bRusCoordConj 
		)
		return;

	if (CanBeDeleted(NodeNo))
	{
		FirstConjNode = m_Nodes[NodeNo];
		DelNode( NodeNo );
	}
};



void CRusSemStructure:: ApplyClauseRulesOnly(vector<PairOfLong>& ClausePairs)
{
 for (long i  =0; i < ClausePairs.size(); i++)
   if (!AreConnectedClauses (ClausePairs[i].first, ClausePairs[i].second)) 
	  {
	    long ClauseNo = ClausePairs[i].first;

		for (size_t RuleNo =0; RuleNo <m_ClauseRules.size(); RuleNo++)
		{
		  if  (m_Clauses[ClausePairs[i].second].m_HostClause != -1) break;

		  if   ( m_ClauseRules[RuleNo].m_Direction == LeftToRight)
		  {
		     if  (( ((*this).*(m_ClauseRules[RuleNo].m_Rule))(RuleNo, ClauseNo, ClausePairs[i].second))) 
			 {
			   m_Clauses[ClausePairs[i].second].m_HostClause = ClauseNo;
 			   m_Clauses[ClausePairs[i].second].m_ClauseRuleNo = RuleNo;
			 };

		     if (m_Clauses[ClausePairs[i].second].m_HostClause != -1) break;
		  }
		  else
 	       if ( ((*this).*(m_ClauseRules[RuleNo].m_Rule))(RuleNo, ClausePairs[i].second, ClauseNo)
			  )
			{
              m_Clauses[ClauseNo].m_HostClause = ClausePairs[i].second;
			  m_Clauses[ClauseNo].m_ClauseRuleNo = RuleNo;

		 	  break;
			};
		}
   };
};


bool  CRusSemStructure:: ClausesHaveCommonSituation (long ClauseNo1, long ClauseNo2) const
{
   vector<long> Nodes1;
   GetClauseRootsWithoutDeleted(ClauseNo1,Nodes1);
   vector<long> Nodes2;
   GetClauseRootsWithoutDeleted(ClauseNo2,Nodes2);
   if (Nodes1.size() == 0) return false;
   if (Nodes2.size() == 0) return false;
   // ����� ��������� ����, ��������� � ������ ����� ������ ������ �����
   const CRusSemNode& RootNode1 = m_Nodes[Nodes1[Nodes1.size() - 1]];
   const CRusSemNode& RootNode2 = m_Nodes[Nodes2[Nodes2.size() - 1]];
   for (long i=0; i < RootNode1.m_ConnectedSits.size(); i++)
	 for (long j=0; j < RootNode2.m_ConnectedSits.size(); j++)
	   if (RootNode1.m_ConnectedSits[i].m_UnitStr == RootNode2.m_ConnectedSits[j].m_UnitStr)
		  return true;
  return false;
};


/*
������������� ������ ���� 
 "�� ����, � ���� � �� ��������� , ��� ��������� ���� �������."
 "�� ��� � ���� ��� ���������, � ����� ���� ���� ��������� � ������, ��������� �� ��������."
 ��� �� �������� ��������� � ������� ������ ��������� ��� ������� (�� ���� �������).
 ��� ���������, ��������� ���������� �� ������.
 ������� ��������� ������������� ������� �� ������ � ��������� ������.

*/
void CRusSemStructure:: CorrectClauseRelations1() 
{
  for (long i=0; i < m_Relations.size(); i++)
	for (long k=i+1; k < m_Relations.size(); k++)
	  if (m_Relations[i].m_TargetNodeNo ==  m_Relations[k].m_TargetNodeNo)
	  if(    (m_Relations[i].m_ClauseRuleNo != -1)
		  && (m_ClauseRules[m_Relations[i].m_ClauseRuleNo].m_Name == "���������")
		)
	  if(    (m_Relations[k].m_ClauseRuleNo != -1)
		  && (m_ClauseRules[m_Relations[k].m_ClauseRuleNo].m_Name == "���������� (������ ������)")
		)
	  {
		  m_Nodes[m_Relations[i].m_TargetNodeNo].m_IncomRelsCount--;
		  m_Relations[i].m_TargetNodeNo = m_Relations[k].m_SourceNodeNo;
		  m_Nodes[m_Relations[i].m_TargetNodeNo].m_IncomRelsCount++;
		  long ClauseNo = m_Nodes[m_Relations[k].m_SourceNodeNo].m_ClauseNo;
		  m_Clauses[ClauseNo].m_HostClause = m_Nodes[m_Relations[i].m_SourceNodeNo].m_ClauseNo;
		  m_Clauses[ClauseNo].m_ClauseRuleNo = m_Relations[i].m_ClauseRuleNo;
	  };
};

void CRusSemStructure:: ApplyClauseRules(long GapSize)
{
	try {
		StartTimer("����������� �����",0);



		SetUseTrue();

		//��������� ��������� ���� ������ �����
		vector<long> Nodes;
		GetAllTextOrderedNodes(Nodes);
		vector<PairOfLong> ClausePairs;

		for (long i =0; i+1  < Nodes.size(); i++)
			if (     (m_Nodes[Nodes[i]].m_ClauseNo !=  m_Nodes[Nodes[i+1]].m_ClauseNo)
				&&  !HaveOnlyPunctBetween (Nodes[i], Nodes[i+1], ';')
				)
				ClausePairs.push_back(PairOfLong(m_Nodes[Nodes[i]].m_ClauseNo, m_Nodes[Nodes[i+1]].m_ClauseNo));

		if (GapSize > 1)
			for (long i  =0; i < m_Clauses.size(); i++)
				for (long k  =i+1; (k < m_Clauses.size()) && (k-i < GapSize); k++)
					ClausePairs.push_back(PairOfLong(i, k));

		for (long i  =0; i < m_Clauses.size(); i++)
		{
			m_Clauses[i].m_HostClause = -1;
			m_Clauses[i].m_ClauseRuleNo = -1;
		};

		ApplyClauseRulesOnly(ClausePairs);

		// �������� �� ���� ����� �����, ������� ������� ����� X1,,,, Xn,������� ������� ��������������
		// �������, ��������, ��� ����� "���� �� ������ � � ����, �� �� ������" 
		// X1 ����� ����� "���� �� ������", � X2 ����� ����� "� � ����".
		// ����� ��������� ������ �� ����� ������� (Xn). ������� ��� ���� <Xn, Y>, ������� 
		// ����� � ���������� �������� ��� ����� (ClausePairs). ������ ����� �������� ��� 
		// NewClausePairs, ���� �������� ���� ���� <X1, Y>. ����� �������, ��� ���� ������ ��� �����  
		// ���� <Xn, Y> ������ ��������� ����� ��� <X1, Y>, ��������� ������� 
		// ��� ���������� ���. ����� �������, �� ��������� ������� �� ������� ����� ����������� ����,
		// ������������ ����� ��������� ����� ����������� ����.
		// ������: "���� �� ������ � � ����, �� �� ������"
		vector<PairOfLong> NewClausePairs;

		for (long i=0; i < ClausePairs.size(); i++)
			if (    AreConnectedClauses (ClausePairs[i].first, ClausePairs[i].second)
				&& (m_Clauses[ClausePairs[i].second].m_ClauseRuleNo != -1)
				&& (m_ClauseRules[m_Clauses[ClausePairs[i].second].m_ClauseRuleNo].m_Name == "���������")
				)
			{
				//���������� ������� X1, Xn, ������ ���� �1 = m_Nodes[BorderNodePairs[i].first].m_ClauseNo
				long Xi = ClausePairs[i].second;
				for (long Cnt=0; Cnt < 10; Cnt++)
				{
					long k=0;
					for (; k < ClausePairs.size(); k++)
						if (      AreConnectedClauses (ClausePairs[k].first, ClausePairs[k].second)
							&& (m_Clauses[ClausePairs[k].second].m_ClauseRuleNo != -1)
							&& (m_ClauseRules[m_Clauses[ClausePairs[k].second].m_ClauseRuleNo].m_Name == "���������")
							&& (Xi ==   ClausePairs[k].first)
							)
							break;

					if ( k == ClausePairs.size()) break;
					Xi = ClausePairs[k].second;
				};

				// ������ �n = �i, ��������� ���� � ������� X1,,,, Xn
				for (long k=0; k < ClausePairs.size(); k++)
					if (      !AreConnectedClauses (ClausePairs[k].first, ClausePairs[k].second)
						&& (Xi = ClausePairs[k].first)
						)
						NewClausePairs.push_back(PairOfLong(ClausePairs[i].first, ClausePairs[k].second));
			};

		ApplyClauseRulesOnly(NewClausePairs);

		// �������� ���� �������, ������� ���������� � �������������� �����, ���� ����� ���������� ����� 
		// ���� ����� ���� ������, ������� ����� � ��� ����� ��������, ����� �������� ������������� ����� � ���� �������,
		// � ������ ����� ���������
		for (int i=0;i < m_Clauses.size(); i++)
			if (GetClauseCoordSimpleConj(i) != -1)
			{
				vector<long> Clauses;
				for (long j=0; j < i; j++)
					/*
					���� if ������ ����������
					*/
					if (    (m_Clauses[j].m_ClauseRuleNo == -1)
						|| (m_ClauseRules[m_Clauses[j].m_ClauseRuleNo].m_Name == "���������")
						)
						if (  ClausesHaveCommonSituation(i, j) )
							Clauses.push_back(j);

				if (Clauses.size() == 1)
				{
					vector<long> Rels;
					GetIncomingClauseRelations(i, Rels);
					DeleteRelations(Rels);
					NewClausePairs.clear();
					NewClausePairs.push_back(PairOfLong(Clauses[0], i));
					m_Clauses[i].m_HostClause = -1;
					m_Nodes[GetClauseCoordSimpleConj(i)].m_bToDelete = false;
					ApplyClauseRulesOnly(NewClausePairs);
				};
			};

		CorrectClauseRelations1();  

		EndTimer("����������� �����");
	}
	catch (...)
	{
		ErrorMessage("ApplyClauseRules failed");
		rml_TRACE ("ApplyClauseRules failed");
		throw;
	};

};



/*
  ������� �� �������� �����:
   1. ������������� ����������: TryClauseDeeprichastie,  TryClausePrichastie,
   2. ���������  ����������� ������������ �� ����������� � ����� ������:
				TryClauseCHTO_WITH_ANIMAT
			    TryClauseCHTOBY_GG 
				TryClauseConjWord
				TryClauseAnaphoricSubordWithoutAntecedent
   3. ���������  ����������� ����������� ������ 
				TryClauseSubordConj
				TryClauseSubordDoubleConj	
				TryClauseCoordSimpleConj
				TryClauseKOTORYJ
   4. ����� �����, ������� ���� ����� ������� (���������� ��� � ������ �������)
   5. ����� �����, ������� �� ����� �������� ����������:
				TryClauseConditionalImperative   
				TryClauseZeroSubordWithoutAntecedent
*/

CRusSemStructure::CRusSemStructure() 
{
	m_SentenceCount = 0;
	m_ClauseConnectionType	    = ProminentClauseRules;
	// �������, �� ����
	{
		CSemanClauseRule N(&CRusSemStructure::TryClauseParenthesis, LeftToRight, "�������");
		m_ClauseRules.push_back(N);
	}

	//"������������� ������������� ��� ���� �������"
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseNSO, LeftToRight, "���"));

	// �� ����, �������
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseParenthesis, RightToLeft, "�������(������ ������)"));

	// � ����, ��������.
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseDeeprichastie, LeftToRight, "������������"));

	// � ����� ����, ���������� ����
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClausePrichastie, LeftToRight, "���������� ������"));

	// ��������, � ����
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseDeeprichastie, RightToLeft, "������������"));

	// �� ����, ��� ����� ���������
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseCHTO_WITH_ANIMAT, LeftToRight, "���+SUB(ANIM)"));
	// � ����� � ���, ��� �� ������ ��� ������("���" - ����), �� ���� ��������������� �����
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseCHTO_TOT_Dop, LeftToRight,"���������� (���+��)"));

	// � �����,  ����� �� ����
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseCHTOBY_GG , LeftToRight, "�����(���)+��"));

	// � ����, ����� �� �������
	// � ���� ��, ��� �� �������
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseConjWord, LeftToRight, "���������� � �����. ������"));



	// � ����, ��� �� ������ ("���" - ���� �����������)
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseAnaphoricSubordWithoutAntecedent, LeftToRight,"���������� (���+��)"));


	// � ������ ���, ������ ��� �� ������ ���.
	//  � ��� ������, ����� �� ������
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseSubordConj, LeftToRight, "���������� � ������"));


	// ���� �� �������� ���, �� � ����.
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseSubordDoubleConj, LeftToRight, "����������(��.����)"));


	// �� ������, � ��  �������
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseCoordSimpleConj, LeftToRight,"���������"));

	// ����� ������ ���, ������� �������� ����, .
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseKOTORYJ, LeftToRight,"����������� (�������)" ));
	// ����� �� ������, � ��� ������ 
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseConjWord, RightToLeft, "���������� (�����.�����, ������ ������)"));

	// ��� �� ������, � ����,  ("���" - ���� �����������)
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseAnaphoricSubordWithoutAntecedent, RightToLeft, "���������� (���+��, ������ ������)"));		 

	// ������ ��� �� ������ ���, � ������ ���.
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseSubordConj, RightToLeft, "���������� (������ ������)"));
	// ���� �� � ����, �� �� ����� �� ������
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseConditionalImperative, LeftToRight, "��������� � �������� ������"));		 
	// � ����, �� ����
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseZeroSubordWithoutAntecedent, LeftToRight, "���������� (0+��)"));		 
	// � ����, �� ������
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryClauseSimpleComma, LeftToRight, "��������� (�������)"));		 
	// � ���� (�� ������)
	m_ClauseRules.push_back(CSemanClauseRule(&CRusSemStructure::TryBracketClause, LeftToRight, "���������� �� ��������"));		 
	m_bShouldBeStopped = false;
	m_AllClausesVariants = 0;
}




string    CRusSemStructure::GetClauseProperiesStr(long ClauseNo) const
{
	string Res = "��������:\n";
	if (m_Clauses[ClauseNo].m_bQuestion)
	{
		Res += "�������������� ������\n";
	};
	return Res;
};

string    CRusSemStructure::GetClauseTreeForTcl() 
{ 
	   //��������� ������ �����
   string Res = "$GT($main,clause_graph) delete nodes\1";

   for (long i=0; i<m_Clauses.size(); i++)
   {
    	Res += Format ("set clause_nds(%i) [$GT($main,clause_graph) create node]\1", i);
		string name;
		if ( IsEmptyClause (i) )
			name = "_empty_";
		else
		 if (m_Clauses[i].m_BeginNodeNo == m_Clauses[i].m_EndNodeNo - 1)
			name = GetNodeStr(m_Clauses[i].m_BeginNodeNo, 20);
		 else
		   if (m_Clauses[i].m_BeginNodeNo+6 >= m_Clauses[i].m_EndNodeNo)
			   for (long k=m_Clauses[i].m_BeginNodeNo; k < m_Clauses[i].m_EndNodeNo; k++)
				   name += GetNodeStr(k, 20) + " ";
          else
		  {   name += GetNodeStr(m_Clauses[i].m_BeginNodeNo, 20) + " ";
		      name += GetNodeStr(m_Clauses[i].m_BeginNodeNo+1, 20) + " .... ";
			  name += GetNodeStr(m_Clauses[i].m_EndNodeNo-2, 20) + " ";
			  name += GetNodeStr(m_Clauses[i].m_EndNodeNo-1, 20);
		  };

		 if (m_Clauses[i].m_bQuestion)
			 name += "?";
		 name +="         ";
		 name =  "         " + name;

        Res += Format ("$GT($main,clause_graph) set $clause_nds(%i) -label \"%s\" -type oval -x 0 -y 0\1", i,  name.c_str());

        Res += Format ("$GT($main,clause_graph) set $clause_nds(%i)  .props \"%s\"\1",i,GetClauseProperiesStr(i).c_str());

   };

   for (long i=0; i<m_Clauses.size(); i++)
   if( m_Clauses[i].m_HostClause != -1 )
   {
	   Res += Format("set edge [$GT($main,clause_graph) create edge $clause_nds(%i) $clause_nds(%i)]\1", m_Clauses[i].m_HostClause, i);

	   if ( m_Clauses[i].m_ClauseRuleNo != -1)
	   {
         string name =   m_ClauseRules[m_Clauses[i].m_ClauseRuleNo].m_Name;
	     Res += Format("$GT($main,clause_graph) set $edge -label \"%s\"\1", name.c_str());
	   };
   };

   return Res;
};

long	CRusSemStructure::GetClauseFirstWordNo(long ClauseNo) const
{
	return m_Nodes[m_Clauses[ClauseNo].m_BeginNodeNo].m_Words[0].m_WordNo;
};

long	CRusSemStructure::GetClauseLastWordNo(long ClauseNo) const
{
  for (int i = m_Clauses[ClauseNo].m_EndNodeNo-1; i >= m_Clauses[ClauseNo].m_BeginNodeNo; i--)
	  if ( m_Nodes[i].IsWordContainer () )
		  return m_Nodes[i].m_Words[m_Nodes[i].m_Words.size() - 1].m_WordNo;

  return -1;

};


void CRusSemStructure::InsertNodeAsSubject (long RootNodeNo, CRusSemNode SubjNode)
{
	long WordNo = m_Nodes[RootNodeNo].m_Words[0].m_WordNo;
	FreeWordNo(WordNo);
	SubjNode.m_Words[0].m_WordNo = WordNo;
	SubjNode.m_ClauseNo = m_Nodes[RootNodeNo].m_ClauseNo;
	CValency V = m_Nodes[RootNodeNo].m_Vals[0];
	/*
	 ��������� �� ��������� ���� ����� RootNodeNo, RootNodeNo ����� ��������� �� �������
	*/
	CRusSemRelation R(V, RootNodeNo+1, InsertNode(SubjNode), "����");
	AddRelation(R);
};

// "�� ������ ����� ���� �� ����, ��� ��������"
// "� �������� ���� �� ��, ��� ������ ������� � ��� �������������
//  ���������������  ���������� �� ���������� ������, ���� ����������� ���.
long CRusSemStructure::CreateDefaultSubjectFromPreviousClause() 
{
	long Bonus  = 0;
	long PrevSubjNodeNo = -1;

	for (long ClauseNo=0;  ClauseNo < m_Clauses.size(); ClauseNo++)
	{
	   vector<long> Roots;
	   GetClauseRoots(ClauseNo,Roots);
	   if (Roots.size() != 1)  
	   { 
		   PrevSubjNodeNo = -1;
		   continue;
	   };
	   long SubjNodeNo = GetRusSubj(Roots[0]);
	   if (    (PrevSubjNodeNo != -1) 
		    && m_Nodes[Roots[0]].IsWordContainer()
			&& (m_Nodes[Roots[0]].m_Vals.size() > 0)
			&& (SubjNodeNo == -1)
			&& HasRichPOS (Roots[0], VERB) // ������������ � ���������� ��������
			&& ( (m_Nodes[Roots[0]].GetGrammems() & rAllTimes) > 0)//������ ���������� ��������
			&& m_pData->GetRusGramTab()->GleicheSubjectPredicate(m_Nodes[PrevSubjNodeNo].m_GramCodes.c_str(), m_Nodes[Roots[0]].m_GramCodes.c_str() ) 
			/* ���� ������ ��������, ����� �� ���� ��������������� ����������*/
			&& (    (m_Clauses[ClauseNo].m_ClauseRuleNo == -1)
			    || (m_ClauseRules[m_Clauses[ClauseNo].m_ClauseRuleNo].m_Name.substr(0,9) != "���������")
			   )

		   )
		{
		    InsertNodeAsSubject(Roots[0], CreateAnaphoricPronoun(PrevSubjNodeNo));

			// ��������� ��� ����� �������� �����������, ��������� �� ����� ����������
			if (!m_AlreadyBuiltClauseVariants.empty() && !m_AlreadyBuiltClauseVariants[0].m_BestLexVariants.empty())
				Bonus -= m_AlreadyBuiltClauseVariants[0].m_BestLexVariants[0].m_BestValue.Coefs->ObligatoryValencyViolationCoef;
		};
	   PrevSubjNodeNo =  SubjNodeNo;
  	};

	// ���� � ������ ����� ��� ��������, ����� 
	// ���������� ������������ ��� �� ���������� �����
	// ��������, "���, ����" ��� 
	// "����������� ��� � �������� ������������ �������"
    vector<long> Roots;
    GetClauseRootsWithoutDeleted(0,Roots);
	if (Roots.size() == 1)  
	{ 
		long SubjNodeNo = GetRusSubj(Roots[0]);
		 if (   (SubjNodeNo == -1)
			  && HasRichPOS(Roots[0], VERB) // ������������ � ���������� ��������
			  && ( (m_Nodes[Roots[0]].GetGrammems() & rAllTimes) == _QM(rPresentTense))//������ ���������� ��������
			  && (m_Nodes[Roots[0]].m_Vals.size() > 0)
			  //  ��� �� ������ ���� �������. ����. ��� "���� � ���!"
			  && !m_Nodes[Roots[0]].HasRelOperator("_���������_����������")
			)
		 {
			 string Lemma;
			 if (m_Nodes[Roots[0]].HasOneGrammem (rFirstPerson)) 
			 {
				 if (m_Nodes[Roots[0]].HasOneGrammem (rSingular) )
					 Lemma = "�";
				else
					Lemma = "��";

				 // ��������� ��� ����� �������� �����������, ��������� �� ����� ����������
				if (!m_AlreadyBuiltClauseVariants.empty() && !m_AlreadyBuiltClauseVariants[0].m_BestLexVariants.empty())
					Bonus -= m_AlreadyBuiltClauseVariants[0].m_BestLexVariants[0].m_BestValue.Coefs->ObligatoryValencyViolationCoef;

				InsertNodeAsSubject(Roots[0], CreatePronounByLemma(Lemma));
			 };

		 }
	};
	
	return Bonus;
};




