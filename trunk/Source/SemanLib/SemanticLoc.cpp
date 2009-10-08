#include "stdafx.h"
#include "SemanticRusStructure.h"

//const MaxQuoteNodeSize = ;


/*
TITL�	= _ _CAT	= GEOSF 	= �����
SF ( � (1)) = �����
GF (1) = �
GF (2)	= 1 ��
	   2 ���
	   3 ��
	   4 ���
EXM	= 1 ���� �����
	   2 ���� Usenet
	   3 ����� www.91.ru
4 ������ "��� ���� �����"   	
*/


bool IsLocSemRel (const string& S)
{
	return (S=="LOK") ||  (S=="TRG-PNT") || (S=="SRC-PNT");
};


bool CRusSemStructure :: IsLocNode(size_t NodeNo) const 
{
  const CRusSemNode& N = m_Nodes[NodeNo];

  bool Result =      N.IsTrueLocNode() 
				||  (    (N.m_Colloc.m_Type  == ThesType)
					 && (N.m_Colloc.GetThesInterp().m_ThesaurusId  == LocThes)
			       )
                   ||  (N.HasGrammems(_QM(rToponym) )
			       )
;
  if (     Result
	    && (N.GetType() == LocRoss) 
	    && GetRossHolder(LocRoss)->HasCX(N.GetUnitNo(),  "�����_����", "D_SF")
		&& !N.HasSomePrep()
     )
  return false; 


  if (     N.HasSomePrep()
	 	&& HasLocPrepInBegining(NodeNo)
		&& (N.m_SemCategory == scObject)
	 )
	 return true;

  if (     N.HasPOS(ADV) // ���, ������
	    && (N.GetType()   != NoneRoss)
	)
		if ( GetRossHolder(N.GetType())->HasCX(N.GetUnitNo(),  "LOK", "D_SEM_REL") )	 
			return true;

  return Result;
};




bool CRusSemStructure :: GetLocSemRel(long NodeNo, string& RelationStr, long& PrepNo) const 
{

	const CSemNode& N = m_Nodes[NodeNo];
    if (N.GetType() == LocRoss) 
	{
if (GetRossHolder(LocRoss)->HasCX(N.GetUnitNo(), "TRG-PNT", "D_SEM_REL"))		 {
RelationStr = "TRG-PNT";			 return  true;
		 };
if (GetRossHolder(LocRoss)->HasCX(N.GetUnitNo(), "SRC-PNT", "D_SEM_REL"))		 {
RelationStr = "SRC-PNT";			 return  true;
		 };
	};

	PrepNo = GetLocPrepFromBegining(NodeNo);
	if (PrepNo != -1)
	{
		RelationStr = GetSemRelOfPrepOrConj(N.m_SynReal.m_Preps[PrepNo]).m_RelationStr;
		return true;
	};
	return false;
};


/*

  1. ������� ����������� �������� � ���������� "�" � "��"
  ���	�	�
   �	in	to
  ��	on	to

TITL�	= ���  _CAT	= GEO GF	= ����
 SF (2) = �����
 GF (2) = 1 �
                  2 �
ENG (1) = in 
// TITL� (1) = � 
//GF (2) = �on 
// TITL� (1) = ��
// GF (2) = �	      to 
// GF (2)= �
 EXM 	= � ���� Linux
	    �� ����� www.ru.91 
*/



string    GetPO (const CDictionary* Ross, WORD UnitNo, long POFieldNo)
{
  if (UnitNo != ErrUnitNo)
  for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= Ross->GetUnitEndPos(UnitNo); i++)
   if    ( (Ross->GetCortegeFieldNo(i) == POFieldNo) 
		 )
			return (const char*)Ross->GetDomItemStr(GetCortege(Ross,i).m_DomItemNos[0]);

   return string("���");
};

struct  CLocHostHypot {
  long m_NodeNo;
  bool m_bOrg;
  long m_Distance;
  long m_bFoundInRoss;
  bool m_UpLw;

  CLocHostHypot (long NodeNo, bool bOrg, long Distance, long bFoundInRoss, bool UpLw)
  {
	  m_NodeNo = NodeNo;
	  m_bOrg = bOrg;
	  m_Distance = Distance;
	  m_bFoundInRoss = bFoundInRoss;
	  m_UpLw = UpLw;
  };
  long GetWeightOfLocHypot () const 
  {  return   m_Distance 
            + (m_bOrg? 1 : 0)*100 
			+ (m_bFoundInRoss ? 0 : 1)*10; 
  };
  bool operator < (const CLocHostHypot & X)  const  {return GetWeightOfLocHypot() < X.GetWeightOfLocHypot(); };
  bool operator == (const CLocHostHypot & X) const {return GetWeightOfLocHypot() == X.GetWeightOfLocHypot(); };


};


long CRusSemStructure::FindLocHost (long NodeNo, long ClauseNo)
{
	vector <CLocHostHypot> HypotNodes;
	for (size_t i=0; i < m_Nodes.size(); i++)
		if	(m_Nodes[i].IsWordContainer())
		if	(IsInClause(i, ClauseNo))
		if	(		(	m_Nodes[i].GetMinWordNo() < m_Nodes[NodeNo].GetMinWordNo()
				&&	(		IsLocNode(i) 
						||	HasSemFet(m_Nodes[i], "ORG")				)
					) 
			)  
	{
		CLocHostHypot Hypot (i, HasSemFet(m_Nodes[i], "ORG"), GetDistance(i, NodeNo),			m_Nodes[i].GetUnitNo() != ErrUnitNo, false);

		HypotNodes.push_back(Hypot);
	};

	sort (HypotNodes.begin(), HypotNodes.end());

	if (HypotNodes.size() == 1) return HypotNodes[0].m_NodeNo;
	if (HypotNodes.size() == 0) return -1;

	for (size_t i = 0; i < HypotNodes.size(); i++)   
	{
		string Q = Format ("Loc Host Weight %i \n", HypotNodes[i].GetWeightOfLocHypot());

		size_t SaveRelSize = m_Relations.size();
		CValency V (string("test_loc"), A_C);
		AddRelation(CRusSemRelation(V, HypotNodes[i].m_NodeNo, NodeNo, ""));

		if (m_Clauses[ClauseNo].m_ClauseSyntaxTop != -1)
		for (size_t k=0; k <m_Nodes.size(); k++)
			if ( IsInClause(k, ClauseNo) )
			if (m_Clauses[ClauseNo].m_ClauseSyntaxTop != k)
			if (GetIncomingRelationsCount(k) == 0)
			{
				CRusSemRelation R (V, m_Clauses[ClauseNo].m_ClauseSyntaxTop, k, "");
				R.m_bSituatRelation = true;
				AddRelation(R);
			};

		bool bProject = IsProjectedNew (SetTagToClause(ClauseNo));

		for (long j = SaveRelSize; j < m_Relations.size(); )
			EraseRelation(SaveRelSize);

		if (bProject) 
			return HypotNodes[i].m_NodeNo; 
	};

	return -1;
};

/*
  1. ���� ����� �� ��1 (SF (��1 = �����) ����� ��2 ��� ����������� �������� ���, 
��� SF (� (��2)) = �����, �� ��1 ������������ �  ��2 � ���� GEO, � ������������������������� ��������� LOK (��1, ��2).  EXM = ������������ ��������� ���������� � ����� ���������

  2. ���� ����� �� ��1 (SF (��1) = �����) ����� ��2 � ���������� ��������� ���, 
��� SF (� (��2)) = �����, �� ��1 ������������ �  ��2 � ���� GEO, � ������������������������� ��������� LOK (��2, ��1).  EXM = � ���� �� ����������    
*/
void    CRusSemStructure :: PutLocRelations(long ClauseNo)
{
	for (long NodeNo=m_Clauses[ClauseNo].m_BeginNodeNo+1; NodeNo < m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
		if  (    (m_Nodes[NodeNo].m_ClauseNo == m_Nodes[NodeNo-1].m_ClauseNo) 
			&& IsLocNode(NodeNo)
			&& IsLocNode(NodeNo-1)
			)
		{
			string RelationStr;
			long PrepNo;
			GetLocSemRel(NodeNo-1, RelationStr, PrepNo);
			CValency V (RelationStr, A_C);
			if (V.m_RelationStr  == "LOK")	   
				if (m_Nodes[NodeNo-1].HasSomePrep())
				{
					AddRelation(CRusSemRelation(V,   NodeNo, NodeNo-1, ""));
					m_Nodes[NodeNo].m_Vals.push_back(V);
				}
				else
				{
					AddRelation(CRusSemRelation(V,   NodeNo-1, NodeNo, ""));
					m_Nodes[NodeNo-1].m_Vals.push_back(V);
				};
		};

};

long  CRusSemStructure :: GetLocInterp(string UnitStr, bool& NegativeForm) const
{
  NegativeForm = false;
  WORD UnitNo = GetRossHolder(LocRoss)->LocateUnit(UnitStr.c_str(), 1);
  if (UnitNo == ErrUnitNo)
	 if (UnitStr.substr(0,2) == "��")
		 {
			 UnitNo = GetRossHolder(LocRoss)->LocateUnit(UnitStr.substr(0, UnitStr.length() - 2).c_str(), 1);
			 if (UnitNo != ErrUnitNo)
				 NegativeForm = true;
		 };
  string PO = GetPO(GetRoss(LocRoss), UnitNo, GetRossHolder(LocRoss)->POFieldNo);
  if (UnitNo != ErrUnitNo)
	   if (   (PO == "���") 
		   || (m_PO == PO) 
		  ) 
		 return UnitNo;
	   
  return  ErrUnitNo;
};



