#include "stdafx.h"
#include "SemanticRusStructure.h"

//const MaxQuoteNodeSize = ;


/*
TITLЕ	= _ _CAT	= GEOSF 	= МЕСТО
SF ( Г (1)) = МЕСТО
GF (1) = С
GF (2)	= 1 Бб
	   2 ИЛЕ
	   3 ЭА
	   4 квч
EXM	= 1 поле СинАн
	   2 сети Usenet
	   3 адрес www.91.ru
4 газета "Нью Йорк Таймс"   	
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
	    && GetRossHolder(LocRoss)->HasCX(N.GetUnitNo(),  "ЧАСТЬ_ТЕЛА", "D_SF")
		&& !N.HasSomePrep()
     )
  return false; 


  if (     N.HasSomePrep()
	 	&& HasLocPrepInBegining(NodeNo)
		&& (N.m_SemCategory == scObject)
	 )
	 return true;

  if (     N.HasPOS(ADV) // где, далеко
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

  1. Правило объединения геогрупп с предлогами "в" и "на"
  рус	П	В
   в	in	to
  на	on	to

TITLЕ	= рус  _CAT	= GEO GF	= ОБСТ
 SF (2) = МЕСТО
 GF (2) = 1 П
                  2 В
ENG (1) = in 
// TITLЕ (1) = в 
//GF (2) = Пon 
// TITLЕ (1) = на
// GF (2) = П	      to 
// GF (2)= В
 EXM 	= в поле Linux
	    на адрес www.ru.91 
*/



string    GetPO (const CDictionary* Ross, WORD UnitNo, long POFieldNo)
{
  if (UnitNo != ErrUnitNo)
  for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= Ross->GetUnitEndPos(UnitNo); i++)
   if    ( (Ross->GetCortegeFieldNo(i) == POFieldNo) 
		 )
			return (const char*)Ross->GetDomItemStr(GetCortege(Ross,i).m_DomItemNos[0]);

   return string("общ");
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
  1. Если слева от Иг1 (SF (Иг1 = МЕСТО) стоит Иг2 без локативного предлога так, 
что SF (Г (Иг2)) = МЕСТО, то Иг1 обьединяется с  Иг2 в одну GEO, а составляющимприписывается отношение LOK (Иг1, Иг2).  EXM = Снохомишская публичная библиотека в штате Вашингтон

  2. Если слева от Иг1 (SF (Иг1) = МЕСТО) стоит Иг2 с локативным предлогом так, 
что SF (Г (Иг2)) = МЕСТО, то Иг1 обьединяется с  Иг2 в одну GEO, а составляющимприписывается отношение LOK (Иг2, Иг1).  EXM = в доме на компьютере    
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
	 if (UnitStr.substr(0,2) == "не")
		 {
			 UnitNo = GetRossHolder(LocRoss)->LocateUnit(UnitStr.substr(0, UnitStr.length() - 2).c_str(), 1);
			 if (UnitNo != ErrUnitNo)
				 NegativeForm = true;
		 };
  string PO = GetPO(GetRoss(LocRoss), UnitNo, GetRossHolder(LocRoss)->POFieldNo);
  if (UnitNo != ErrUnitNo)
	   if (   (PO == "общ") 
		   || (m_PO == PO) 
		  ) 
		 return UnitNo;
	   
  return  ErrUnitNo;
};



