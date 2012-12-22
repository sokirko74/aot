#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"

//#pragma optimize( "", off )

//#define Trace 0
#define Trace 1


// ��������� ���� GFi(j). ����� ����� ��������� ������ �� ������ �� ��������, �������  
// �������� "(���)" ��� ����� ������ Copul.
// ����  �������� GFi(j)  ���������� �� ������ "(���)", �� ��� ��������, ��� ��� �������������� ��������������
// ����� ������������ ������ ��� ��������� �����������.

bool CRusSemStructure::LoadFromDictForPassive(WORD UnitNo, BYTE LeafId, BYTE BracketLeafId, CSemPattern& P)
{
	assert (UnitNo != ErrUnitNo);

	P.InitSemPattern(GetRossHolder(Ross), UnitNo, LeafId, BracketLeafId);
	P.LoadGramFromDict();
	
	for (size_t i=0; i < P.m_GramCorteges.size(); i++)
		if (P.m_GramCorteges[i].m_DomItemNos[0] != P.m_pRossDoc->CopulNo)
			if (P.m_GramCorteges[i].m_DomItemNos[0] != P.m_pRossDoc->PassiveNo)
			{
				P.m_GramCorteges.erase(P.m_GramCorteges.begin() + i );
				i--;
			}
			else
				for (size_t k=0; k < 9; k++) 
					P.m_GramCorteges[i].m_DomItemNos[k] = P.m_GramCorteges[i].m_DomItemNos[k+1];

	return    P.m_GramCorteges.empty();
};

// �������, ������� ���������� ��������� ��������� ��� ��������� ����� �������
// (CDictUnitInterp::m_bPassiveForm)
void  CRusSemStructure::InitPassivePattern(size_t NodeNo, BYTE ValencyNo, CSemPattern& P)
{
	if (m_Nodes[NodeNo].GetType() != Ross) return;

	WORD UnitNo = m_Nodes[NodeNo].GetUnitNo();

	assert (UnitNo != ErrUnitNo);

	if   (ValencyNo == 0)
	{
		if (m_Nodes[NodeNo].m_Vals.size()< 2) return;
		LoadFromDictForPassive(UnitNo, 2, 0, P);
		P.m_PatternValency = m_Nodes[NodeNo].m_Vals[1];

		if (P.m_GramCorteges.size() == 0)
			P.m_GramCorteges.push_back(GetSubjCortege(P.m_pRossDoc));

		/*
		"����������, ��� ����������������� ���������� 55,5 ���."
		�����  ��� ������ ������ �������� ����� �����������. ������� ��� ����������� 
		GF2 ������ ������� � �����������.	  */
		CSemPattern Q;
		Q.InitSemPattern(GetRossHolder(Ross), UnitNo, 2, 0);
		Q.LoadGramFromDict();
		for (long i=0; i < Q.m_GramCorteges.size(); i++)
		{
			string SynRel = Q.GetSynRelStr(i);
			string SynFet = Q.GetSynFetStr(i);
			if (  (SynRel ==  "����_��")
				||(    (SynFet.length() > 4)
				&& (    (SynFet.substr(SynFet.length()-3) ==  "+��")
				||  (SynFet.substr(SynFet.length()-4) ==  "+���")
				)
				)
			 )
			{
				P.m_GramCorteges.push_back(Q.m_GramCorteges[i]);

			};
		};
	}
	else if (ValencyNo == 1)
	{
		if (m_Nodes[NodeNo].m_Vals.size() < 1) return;
		LoadFromDictForPassive(UnitNo, 1, 0, P);
		P.m_PatternValency = m_Nodes[NodeNo].m_Vals[0];
		if (P.m_GramCorteges.empty())
			P.m_GramCorteges.push_back(GetInstrObj(P.m_pRossDoc));
		P.m_PatternValency.m_InstrAgent = true;
	}
	else 
	{
		P.m_PatternValency = m_Nodes[NodeNo].m_Vals[ValencyNo];
		P.InitSemPattern(GetRossHolder(Ross), UnitNo, m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, m_Nodes[NodeNo].m_Vals[ValencyNo].m_BracketLeafId);
		P.LoadGramFromDict();
	};

};






inline TCortege GetFullGleicheRightCortege (const CRossHolder* RossDoc, BYTE LeafId)
{
    TCortege C;
	C.m_FieldNo = RossDoc->GramFetFieldNo;;
	C.m_LeafId = LeafId;
	C.m_LevelId = 0;
	C.m_DomItemNos[0] = RossDoc->RightDirectionNo;
	C.m_DomItemNos[1] = RossDoc->ReverseSynONo;
	C.m_DomItemNos[2] = RossDoc->NounGrpNo;
	return C;
};



void  CRusSemStructure::GetActantPattern(size_t NodeNo, BYTE ValencyNo, CSemPattern& P)
{
	assert (ValencyNo < m_Nodes[NodeNo].m_Vals.size());

	P.m_SourceNo = NodeNo;
	P.m_GramCorteges.clear();
	P.m_LexFets.clear();

	if (m_Nodes[NodeNo].GetType() != NoneRoss)
		P.m_pRossDoc = GetRossHolder(m_Nodes[NodeNo].GetType());
	else
		P.m_pRossDoc = GetRossHolder(Ross); // �� ��������� ��� � ����

	P.m_PatternValency = m_Nodes[NodeNo].m_Vals[ValencyNo];

	if  (     (HasRichPOS(P.m_SourceNo, ADJ_SHORT))
			&& (ValencyNo == 0)
		)
	{
		// ��������� ������ ����������� ��� �������� ���������������
		if (m_Nodes[P.m_SourceNo].m_Vals.size() >= 1) 
		{
			P.m_GramCorteges.push_back(GetSubjCortege(P.m_pRossDoc));
			P.m_PatternValency = m_Nodes[P.m_SourceNo].m_Vals[0];
		}
	}
	else 
		if (m_Nodes[NodeNo].GetUnitNo() != ErrUnitNo)
		{
			if  (     m_Nodes[NodeNo].IsPassiveVerb() && (ValencyNo < 2) )
				InitPassivePattern(NodeNo, ValencyNo, P);
			else
				if (!P.m_PatternValency.m_IsAddition)
				{
					P.InitSemPattern(P.m_pRossDoc, m_Nodes[NodeNo].GetUnitNo(), P.m_PatternValency.m_LeafId, P.m_PatternValency.m_BracketLeafId);
					P.LoadGramFromDict();
				}
				else
				{
					P.InitSemPattern(GetRossHolder(Ross), P.m_PatternValency.m_UnitNo, P.m_PatternValency.m_LeafId, P.m_PatternValency.m_BracketLeafId);
					P.LoadGramFromDict();
				}
		}

		/*
		�������������� GF ���������, ������� �� �������� ��������� �����. ��� �����,
		����� �������� ��������� ����� �� � ����������, ������� ����� � ����������.
		��������, "������������� � ������ ������ ����������"
		���� ���������  �������������, ����� ������ �����������
		�������� "> X!:��", ����� ������ ����������� ��������  "> X!:��"
		*/
		if (m_Clauses[m_Nodes[NodeNo].m_ClauseNo].m_ClauseSyntaxTop != NodeNo)
			if (HasRichPOS(NodeNo,PARTICIPLE))
				if (m_Nodes[NodeNo].HasOneGrammem (rActiveVoice) )
				{
					if (ValencyNo == 0)
					{
						P.m_GramCorteges.clear();
						P.m_GramCorteges.push_back(GetFullGleicheRightCortege(P.m_pRossDoc, 1));
					};
				}
				else
				if (m_Nodes[NodeNo].HasOneGrammem ( rPassiveVoice ))
				{
					/*
					��� ���������� ��������� (��������, "�����������"), ���� ����������� 
					�������� ������ �����������, ������ ����� �������������� �� � > X!:��,
					��������:
					"����������� �������  ���������"
					*/
					for (long i = 0; i < P.m_GramCorteges.size(); i++)
						if (P.GetSynRel(i) == P.m_pRossDoc->DirectObjSynONo)
						{
							P.m_GramCorteges.clear();
							P.m_GramCorteges.push_back(GetFullGleicheRightCortege(P.m_pRossDoc, 2));
							break;
						};
				}

	P.LoadSemFromDict();

};



bool IsEqualWithPhonetics (const string& TextItem,  const string& FormatItem)
{
	return         (TextItem ==  FormatItem)
		        || (FormatItem == "*")
		        || ( (TextItem == "��") && (FormatItem == "�") )
				|| ( (TextItem == "��") && (FormatItem == "�") )
				|| ( (TextItem == "��") && (FormatItem == "�") )
				|| ( (TextItem == "����") && (FormatItem == "���") )
				|| ( (TextItem == "���") && (FormatItem == "��") )
				|| ( (TextItem== "������") && (FormatItem == "�����") )
				|| ( (TextItem == "��") && (FormatItem == "�") )				
				|| ( (TextItem == "���") && (FormatItem == "�") )
				|| ( (TextItem == "���") && (FormatItem == "OBJ") )				
				|| ( (TextItem == "����") && (FormatItem == "���") )
				|| ( (TextItem == "���") && (FormatItem == "��") );
}




bool CRusSemStructure::CheckSemanOfPrep(string RelationStr, bool IsAdditionValency, long NodeNo, long&  PrepNo)
{
   assert (m_Nodes[NodeNo].HasSomePrep());

   PrepNo = -1;

   if     (     IsLocSemRel(RelationStr) )
   {
   	   if (IsAdditionValency && !IsLocNode(NodeNo)) return false;

	   string    RelStr;
		// ������������ IN-DIRECT � TRG-PNT
	   if (RelationStr == "IN-DIRECT") RelationStr = "TRG-PNT";	   
	   for (long i=0; i <m_Nodes[NodeNo].m_SynReal.m_Preps.size(); i++)
	   {
		  long UnitNo = m_Nodes[NodeNo].m_SynReal.m_Preps[i].m_UnitNo;
	   	  if  (		(HasItem(OborRoss, UnitNo, "SF",RelationStr,"D_SEM_REL", 0 , 0))
				||	(   (		RelationStr == "IN-DIRECT" )
							&&	(HasItem(OborRoss, UnitNo, "SF","TRG-PNT","D_SEM_REL", 0 , 0))				   
						)
				||	(		( RelationStr == "TRG-PNT" )
						&&	(HasItem(OborRoss, UnitNo, "SF","IN-DIRECT","D_SEM_REL", 0 , 0))				  
					)
			  )
		  { 
		     PrepNo = i;
			 return true;
		  };
	   };
	   return false;
   }
   else
	if (RelationStr == "TIME")		  
		return m_Nodes[NodeNo].IsMainTimeRossNode();
	else 
		return true;
};

bool CRusSemStructure::CheckDirection (long NodeNo1, long NodeNo2, string Direction)  const 
{
  if (   Direction == ">>" )
  {
	  if (NodeNo1 >= m_Clauses[ m_Nodes[NodeNo1].m_ClauseNo ].m_EndNodeNo  - 1)
		  return false;
	  long nd_curr = GetDefiniteRightHostByNotWeakSynRelation(NodeNo1+1);
	  if (nd_curr != NodeNo2) return false;
  }
  else
  if ( Direction == ">" )
  {
	  if (m_Nodes[NodeNo2].GetMinWordNo() < m_Nodes[NodeNo1].GetMinWordNo()) return false;
  }
  else
  if ( Direction == "<" )
  {
	  if (m_Nodes[NodeNo1].GetMinWordNo() < m_Nodes[NodeNo2].GetMinWordNo()) return false;
  }
  else
  if (   Direction == "<<" )
  {
	  long u = m_Nodes[NodeNo1].GetMinWordNo() - m_Nodes[NodeNo2].GetMaxWordNo();
      if  ((u < 0) || (u > 1)) return false;
  }
  else
  if (   Direction == "^" )
  {
      if    (   (m_Nodes[NodeNo2].GetMaxWordNo() > m_Nodes[NodeNo1].GetMaxWordNo())
		     || (m_Nodes[NodeNo1].GetMinWordNo() > m_Nodes[NodeNo2].GetMaxWordNo()))
      return false; 
  }
  return true;
};


bool CRusSemStructure::CheckPatternReverseGramFetLine (CSemPattern& P,  CSynRealization& SynRealization, size_t NodeNo) 
{
	 long SynFet = P.GetSynFet(SynRealization.m_CortegeNo);
	 string SynFetStr  = P.GetSynFetStr(SynRealization.m_CortegeNo);
	 TCortege& C = P.m_GramCorteges[SynRealization.m_CortegeNo];

	 if  (   m_Nodes[P.m_SourceNo].m_bFullGleiche
		  || HasRichPOS (P.m_SourceNo, ADV) 
		 )
	   if (   SynFetStr == "����"
		   || SynFetStr == "��"
		  )
		 {
		    /* ����� �����  ��������������� � ��������������  �� ����� ���� �����  �������.
			   (�.�. ������� ��������� ����� �������������� � ���������������)
			   ��������, "�������� � ����",   ����� "��������" �� ����� ���������� � "����",
			   ���:  "���� � ��������"
			*/
		    long nd1 = NodeNo; 
			long nd2 = P.m_SourceNo;
			if (nd2 < nd1) swap (nd1, nd2);
			if (    m_Nodes[nd2].HasSomePrep()  
				&&  (m_Nodes[nd1].GetMinWordNo() < m_Nodes[nd2].m_PrepWordNo) 
				&&  (m_Nodes[nd2].m_PrepWordNo < m_Nodes[nd2].GetMinWordNo()) // �����������
			   )
			   return false;

			if (m_Nodes[P.m_SourceNo].m_bFullGleiche)
				if ( !FullGleiche(NodeNo, P.m_SourceNo) )
				 return false;
		 };
	 // ����, ���
	 // � ���������, � ������������ (� � ����,� ���������) 
	  //�� ����� ��������  ���� � ����������, ������� ���������� 
	 // ��������� ��� �������
 	 if (HasRichPOS(P.m_SourceNo, NUMERAL))
	 {
		 if (m_Nodes[P.m_SourceNo].GetGrammems() & rAllGenders)
		   if ((m_Nodes[P.m_SourceNo].GetGrammems() & rAllGenders & m_Nodes[NodeNo].GetGrammems()) == 0)
			   return false;

		  if (   m_Nodes[P.m_SourceNo].IsPrimitive() )
		  {
			  if (m_Nodes[P.m_SourceNo].m_Words[0].m_Lemma == "����")
				 if ( !m_Nodes[P.m_SourceNo].HasOneGrammem (rSingular) )
					 return false;

			  if (m_Nodes[P.m_SourceNo].m_Words[0].m_Lemma == "���")
			     if ( !m_Nodes[P.m_SourceNo].HasOneGrammem (rPlural) )
				     return false;
		  };
	 };
 	


 
	 // � ������ COMPARATIVE_T  ������������� ������� ������ ���� �������� �����
	if (m_Nodes[NodeNo].m_bCompAdj)
	 if (SynFetStr == "��")
	  if (m_Clauses[m_Nodes[NodeNo].m_ClauseNo].m_ClauseType == COMPARATIVE_T)
		  return true;

	  
	// ��������� ������ �������������� � ������������
	if (    m_Nodes[P.m_SourceNo].m_bCompAdj 
		&&  !HasRichPOS(P.m_SourceNo, NUMERAL)
	   )
	  if (P.m_PatternValency.m_LeafId == 1)
	    return  false;


     // ��� ������������� �������� ���� ��������� �������  (BuildOutcomingOf_CHEM_Obor � BuildOutcomingOf_COMP_Obor)
	 // ������ ������ ����������� GF ������ ��� �������������� �������� (��������� � �������)
	 if (!P.m_pRossDoc->IsCompAdjCortege(C))
	    if (    m_Nodes[P.m_SourceNo].m_bCompAdj
			&&  !HasRichPOS(P.m_SourceNo, NUMERAL)
			)  return false;

	if ( IsVerbForm (m_Nodes[NodeNo]) ) 
	  if (SynFetStr == "��_���")
		  return   m_Nodes[NodeNo].HasOneGrammem (rNonPerfective);
	  else
		 if (SynFetStr == "��_��")
			 return  m_Nodes[NodeNo].HasOneGrammem (rPerfective);
		 else
 		   if (SynFetStr == "��_����")
			 return  !HasRichPOS (NodeNo,INFINITIVE);
  			else
 		      if (SynFetStr == "���")
			   return  HasRichPOS (NodeNo,INFINITIVE);
			  else
   		        if (SynFetStr == "����_���")
		          return  m_Nodes[NodeNo].HasRelOperator("��");
				else
				  if ( (SynFetStr == "��") || (SynFetStr == "����"))
					 return  true;

	
	// �� ����� ���������� ������� ��������������
	if (HasRichPOS(NodeNo, ADJ_SHORT))
	 if ( (SynFetStr == "��") || (SynFetStr == "����"))
		  return true;

	if (HasRichPOS (NodeNo, PREDK))	
		if ( (SynFetStr == "��") || (SynFetStr == "����"))
				return  true;

    if (HasRichPOS(NodeNo, ADJ_SHORT))
       if (SynFetStr == "��_����")  
		  return  true;
	 else
	    if (SynFetStr == "����_���")
		    return  m_Nodes[NodeNo].HasRelOperator("��");


	if (SynFetStr == "��")
	  return  HasRichPOS(NodeNo, NOUN) || HasPOS(NodeNo, PRONOUN);

	// "����� ��������"
	if (SynFetStr == "����")
	  return  HasRichPOS(NodeNo, ADJ_FULL);

	// ����� ����� ����� ���������� ��������. ������, �� ���� ���� ��������� ����� ��� ������.
	// ������  ��� ��������� ������ ��� ���� ���� �������.
   if (SynFetStr == "�_��")
   {
	   // �� ������ ���������� ����������� ���� "������� � ����",
	   // ��� "�������" �������� �������� �� �_��, � "����" - ��� ��, ��
	  if (m_Nodes[NodeNo].HasSomePrep())
		  if (P.m_SourceNo < NodeNo)
			  return false;
	  return    (HasRichPOS(NodeNo, NOUN) || HasPOS(NodeNo, PRONOUN)) 
	         && ( ( m_Nodes[NodeNo].GetInnerGrammems() & ( _QM(rSingular) |  _QM(rGenitiv) )) == ( _QM(rSingular) |  _QM(rGenitiv) ));
   };

   if (SynFetStr == "�_��")
   {
   	  // �� ������ ���������� ����������� ���� "������� � ����",
	  // ��� "�������" �������� �������� �� �_��, � "����" - ��� ��, ��
	  if (m_Nodes[NodeNo].HasSomePrep())
		  if (P.m_SourceNo < NodeNo)
			  return false;

	  return    (HasRichPOS(NodeNo, NOUN) || HasRichPOS(NodeNo, PRONOUN)) 
	         && ( (m_Nodes[NodeNo].GetInnerGrammems() & ( _QM(rPlural) |  _QM(rGenitiv) )) == ( _QM(rPlural) |  _QM(rGenitiv))  );
   };

	return false;
};


bool CRusSemStructure::IsHumanName (const CRusSemNode& N)  const
{
    return		(N.m_SynGroupTypeStr == NAMES)
		   ||	N.HasOneGrammem(rSurName) 
	       ||	N.HasOneGrammem(rName);
};


bool CRusSemStructure::CanBeSubjectForImperative (long NodeNo, long SourceNodeNo)  const
{
	/*
	 ����� ����� ����������� ������ ���� "���� �� � ����, �� �� ����� �� ������"
	 ���� ���������, ��� ���������� ����� ����� ����� ����������, � ����� ���� 
	 ��� ������, ������� ����� "��"
	*/
	bool bDir = CheckDirection(SourceNodeNo, NodeNo, ">>");

	long ClauseNo = m_Nodes[NodeNo].m_ClauseNo+1;

	for (; ClauseNo < m_Clauses.size(); ClauseNo++)
		if (m_Clauses[ClauseNo].m_HasParticleBY)
			break;

	if (   bDir && ClauseNo < m_Clauses.size() )
	   return true;

	const CRusSemNode& N = m_Nodes[NodeNo];
	return     N.IsLemma("����")  // ���� ���� ������
		   ||  (bDir && N.IsLemma("��"))    // ��� �� � ����
		   ||  N.IsLemma("����")    // ���� ������� ����������.
		   ||  (bDir && N.IsLemma("��"));   // ����� �� � ����
};

bool CanBeMorphSubject (const CRusSemNode& Node )
{
	if (Node.IsPrimitive())
	{
		// "������ �� ���������"
		if (Node.m_Words[0].m_Word == "������") return true;
		// "����� �� ������"
		if (Node.m_Words[0].m_Word == "�����") return true;
	};
     if (	  !Node.HasOneGrammem(rNominativ)
		   || Node.IsLemma("���")
		   || Node.IsLemma("��") 
		   || Node.IsLemma("��") 
		   || (     Node.HasPOS(PRONOUN_P) 
				&& (   !Node.IsPrimitive()
					|| !Node.m_Words[0].IsRusSubstPronounP()
				   )
				  
			  )
		)
		return false;

	return true;
}

bool CRusSemStructure::CheckPatternGramFetLine (CSemPattern& P,  CSynRealization& SynRealization, size_t NodeNo) 
{
	SynRealization.SetEmpty();

	TCortege& C = P.m_GramCorteges[SynRealization.m_CortegeNo];


	if (P.m_pRossDoc->IsVerbFet(C.m_DomItemNos[0]))
		if (C.m_DomItemNos[0] == P.m_pRossDoc->NegativeNo) 	
			if (!m_Nodes[P.m_SourceNo].HasRelOperator("��"))
				return false;

	if (P.m_pRossDoc->IsPosition(C.m_DomItemNos[0]))
		if (!CheckDirection  (P.m_SourceNo, NodeNo, P.m_pRossDoc->GetDomItemStrInner(C.m_DomItemNos[0])) )
			return false;


	if (P.m_pRossDoc->IsCompAdjCortege(C))
		if ( !m_Nodes[NodeNo].HasOneGrammem(rComparative) )
			return false;

	if ( P.HasSemFet("KEY") )	  
		if (m_Nodes[NodeNo].m_SynGroupTypeStr == KEYB)
			return true;

	if  (P.m_PatternValency.m_RelationStr == "NAME")    
		if(!IsHumanName(m_Nodes[NodeNo]) )
			return false;



	if  (P.m_PatternValency.m_RelationStr == "IDENT")	  
		if	(		(		(m_Nodes[NodeNo].m_Words[0].m_CharCase == LowLow)
						&&	!m_Nodes[NodeNo].m_Words[0].m_ILE
						&&	!m_Nodes[NodeNo].m_bQuoteMarks
					)
				||  m_Nodes[NodeNo].HasSomePrep()
				||  IsTimeAdverbial(NodeNo)
				|| (m_Nodes[NodeNo].GetMinWordNo() < m_Nodes[P.m_SourceNo].GetMinWordNo())
				||  ( IsHumanName(m_Nodes[NodeNo]) &&	!m_Nodes[NodeNo].m_bQuoteMarks) // �������� "������ ���������"
			)
			return false;

	// ��������� �� ����� ����� ����������� �� ����������
	if (     (C.m_DomItemNos[0] == P.m_pRossDoc->SubjSynONo) 
		&& HasRichPOS(P.m_SourceNo, PARTICIPLE)
		)
		return false;

	//� ���������� ��� ����������� �� ����������
	if  (    (C.m_DomItemNos[0] == P.m_pRossDoc->SubjSynONo) 
		&& (     HasRichPOS(P.m_SourceNo, INFINITIVE)
		||  HasRichPOS(P.m_SourceNo, ADVERB_PARTICIPLE)
		)

		)
		return false;

	/*
	���������, ��� ������ ��������� � ������ �������, ����
	���� ������� ��� ���������
	*/
	if(    m_Nodes[NodeNo].IsWordContainer()
		&& m_Nodes[P.m_SourceNo].IsWordContainer()
		&& (m_Nodes[P.m_SourceNo].m_MinActantWordNo != -1)
		)
	{
		long WordNo = m_Nodes[NodeNo].m_Words[0].m_SynWordNo;
		if (! (   (m_Nodes[P.m_SourceNo].m_MinActantWordNo < WordNo)
			&& ( WordNo < m_Nodes[P.m_SourceNo].m_MaxActantWordNo)
			)
			)
			return false;
	};


	// ����  ����� �������, ���  � ��� ���������  ���� 
	//  ���_���:��������� 
	//  ���_���:MUA:���������  //  ������� ���_���:���������
	long SynRelNo = P.GetSynRel(SynRealization.m_CortegeNo);
	string SynRelStr  = P.m_pRossDoc->GetDomItemStrInner(SynRelNo);
	long SynFet = P.GetSynFet(SynRealization.m_CortegeNo);
	string SynFetStr  = P.GetSynFetStr(SynRealization.m_CortegeNo);
	const CRusSemClause& Clause = m_Clauses[m_Nodes[NodeNo].m_ClauseNo];
	if (P.m_pRossDoc->ReverseSynONo == SynRelNo)
		return CheckPatternReverseGramFetLine (P,  SynRealization, NodeNo);


	/*
	������������� ������ ��� "����, �������� � ���������, ������ �����"
	*/

	if ( SynRelStr == "�����" )	
		if ( SynFetStr == "����" )	
			if ( HasRichPOS(P.m_SourceNo, NOUN) )
				if ( HasRichPOS(NodeNo, ADJ_FULL)  )
				{

					if ( Clause.GetGramMatrix(NodeNo, P.m_SourceNo) & HaveCommaBetween )
						if ( !HaveNounNodeBetweenInOneClause(NodeNo, P.m_SourceNo) )
							if ( FullGleiche(NodeNo, P.m_SourceNo) )
								return true;
				};

	/*
	�� ����� ������ ��������� ����� ������,������� ��������� ������ ���� �������,
	�����  �������
	1. �������� ������� ��� ����� "��, ������, ����"
	2. ��������� ("����, �������� � ���������, ������ �����")
	(���������� ������)
	*/
	if (   m_Nodes[P.m_SourceNo].IsWordContainer()
		&& (Clause.GetGramMatrix(P.m_SourceNo, NodeNo) & HaveOnlyCommaBetween)  // ������ ���� �������
		&& !m_Nodes[P.m_SourceNo].m_bSaveIsParenthesis	// ��� �� ������� ������
		)  
		return false;


	if	(     IsTimeAdverbial(NodeNo)
		&&	( P.m_PatternValency.m_RelationStr != "TIME")	   /* ��������� ���������� ����� ��������� �� ������ ����������� �������*/
		&&		(		!m_Nodes[NodeNo].IsMainTimeRossNode()
		||	!m_pData->m_TimeUnits[m_Nodes[NodeNo].m_Colloc.GetRossInterp().m_UnitNo].m_bCanFillNotTimeValency
		)
		)
	{
		return false;
	};

	// ���������� ������ �� ����� ��������� �� ���, ��� ���� �� ���,
	// ����� ������ c ������������ ������������ ���� "������", �������� � ����� "�� � ���", 
	if (    m_Nodes[P.m_SourceNo].HasSomePrep() 
		&&  m_Nodes[NodeNo].IsWordContainer()	
		&&  (m_Nodes[NodeNo].GetMinWordNo() < m_Nodes[P.m_SourceNo].GetMinWordNo())
		&&  !HasRichPOS(NodeNo, ADJ_FULL) 
		&&  !HasRichPOS(NodeNo, NUMERAL) 
		&&  !HasRichPOS(NodeNo, PRONOUN_P)
		)
		return false;



	if (   (SynRelNo == P.m_pRossDoc->DirectObjSynONo)
		&& (SynFet == P.m_pRossDoc->AccusativNo)  
		&& m_Nodes[NodeNo].HasOneGrammem(rGenitiv)
		)
		// ���������� ������ � ���������� ����� ��������� � ������� �����������,
		if (     m_Nodes[P.m_SourceNo].HasRelOperator("��") 

			//  ������ ������� "��" � ������� ��������� ����������� ������� ��������� 
			//  � ������� ������������, � �� ������������, ��������:
			//  "�� �� ������ �������������"
			|| m_Clauses[m_Nodes[P.m_SourceNo].m_ClauseNo].m_bSaveClauseHasNegationInVerb
			)
			SynFet = P.m_pRossDoc->GenitivNo;

	if (   (SynRelNo == P.m_pRossDoc->DirectObjSynONo)
		&& (SynFet == P.m_pRossDoc->AccusativNo)  
		)
		if (     m_Nodes[NodeNo].IsWordContainer()
			&&  (m_Nodes[NodeNo].m_MainWordNo != -1)
			&&  (       (m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_Lemma == "������")
			||    (m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_Lemma == "������")
			)
			&&  m_Nodes[NodeNo].HasOneGrammem(rGenitiv)
			)
			SynFet = P.m_pRossDoc->GenitivNo;


	if ( SynRelStr == "�_���" )	
		if ( SynFetStr == "����" )	
			return false;



	if (SynRelNo == P.m_pRossDoc->SubjSynONo)
	{
		if (m_Nodes[NodeNo].HasSomePrep()) return false;
		if (HasIncomingNotWeakSynRelation (NodeNo)) return false;
		long SourceNodeNo = P.m_SourceNo;


		if (m_Nodes[P.m_SourceNo].m_NodeType != Copul)
			if (SynFet !=  P.m_pRossDoc->InfinitiveNo)
				if (!CanBeMorphSubject(m_Nodes[NodeNo]) )
					return false;
				else
				{
					if  (    (SynFetStr == "�_��") 
						&& ! m_Nodes[NodeNo].HasOneGrammem (rPlural)
						)
						return false;
					/*
					���� ������������� ���������� �� ���������� �������������� ����������,
					����� ����� ��������� ����������� �� ������� ��� �������������� ����������
					*/
					if (   m_Nodes[P.m_SourceNo].HasOneGrammem(rImperative)
						&& !CanBeSubjectForImperative(NodeNo, P.m_SourceNo)
						&& ((m_Nodes[P.m_SourceNo].GetGrammems() & rAllTimes) == 0)
						)
						return false;
					//"���� � ���� ������"
					if( m_Nodes[P.m_SourceNo].HasPOS(VERB) && ((m_Nodes[NodeNo].HasPOS(NOUN)	
						&& m_pData->GetRusGramTab()->ConflictGenderNumber(m_Nodes[P.m_SourceNo].m_GramCodes.c_str(),m_Nodes[NodeNo].m_GramCodes.c_str()))
						|| (m_Nodes[NodeNo].m_NodeType == MNA && ((m_Nodes[P.m_SourceNo].GetGrammems() & rAllNumbers) == _QM(rSingular) ) ) )
						)
							//m_Nodes[NodeNo].m_NodeType == MNA ? m_pData->GetRusGramTab()->GetGramCodes(NOUN, _QM(rPlural) | _QM(rNominativ), CaseNumber).c_str()
							//: 
						return false;
					bool xx = m_Nodes[NodeNo].m_NodeType == MNA ;
					xx = ((m_Nodes[P.m_SourceNo].GetGrammems() & rAllNumbers) == _QM(rSingular) );
					xx= m_Nodes[P.m_SourceNo].HasPOS(VERB);
					SynRealization.m_Grammems = _QM(rNominativ);
					return true;
				}
			else 
				if (HasRichPOS(NodeNo, INFINITIVE))
				{
					SynRealization.m_Grammems = 0;
					return true;
				};
		return false;;
	}

	if (     (m_Nodes[P.m_SourceNo].m_SemCategory != scSituation)
		&&  HasRichPOS(P.m_SourceNo, NOUN)  
		&&  HasRichPOS(NodeNo, PRONOUN)  
		&&  (SynFet == P.m_pRossDoc->GenitivNo)
		)
		return false;

	// ��������������� �� ����� ���������  ������� �� ���� ����� ���������������
	// ���� ���������� �� �������� �����������, ����� ��� �� ��������
	// �������������� ��� �����������, � ����������� ��������� ���� ����.  
	if (   
		(    ( SynRelStr == "�_���") 
		|| ( SynRelStr == "����_��")
		)
		&&    HasRichPOS(P.m_SourceNo, NOUN) 
		&&  (     HasRichPOS(NodeNo, PRONOUN)  
		|| HasRichPOS(NodeNo, NOUN) 
		|| HasRichPOS(NodeNo, NUMERAL) 
		|| HasRichPOS(NodeNo, NUMERAL_P) 
		|| HasRichPOS(NodeNo, PRONOUN_P) 
		)
		&&	(		!IsTimeAdverbial(NodeNo)
		||	!(P.m_PatternValency.m_RelationStr == "TIME")		   
		)
		&&  (P.m_SourceNo > NodeNo)
		)
		return false;



	// GF = �_��� : ����_�����
	if (SynRelStr == "�_���")
		if (SynFetStr == "����_�����")
			if (     IsPossesivePronoun(m_Nodes[NodeNo])
				|| (     m_Nodes[NodeNo].IsPrimitive()
				&& (m_Nodes[NodeNo].m_Words[0].m_Lemma == "���")
			 )
			 )
			 if (  (      FullGleiche(P.m_SourceNo, NodeNo) 
				 &&   (GetSynRelationsCount(NodeNo)  == 0)
				 )
				 ||     
				 _find(m_SynRelations, CSynRelation (P.m_SourceNo, NodeNo, "����_���"))
				 )
			 {
				 //SynRealization.m_SynRelation = NOUN_ADJ;
				 return true;
			 }
			 else
				 return false;

	// GF = ����-��� : *
	if (SynRelStr == "����_���")	
		if (_find(m_SynRelations, CSynRelation(P.m_SourceNo, NodeNo, "����_���")))
		{
			return true;
		}
		else
			return false;



	if ( SynRelStr == "�_���" )	
		if ( SynFetStr == "����" )	
			if ( !HaveNounNodeBetweenInOneClause(NodeNo, P.m_SourceNo) )
				if ( HasRichPOS(NodeNo, ADJ_FULL)   || HasPOS(NodeNo, PRONOUN_P)   )
					if ( FullGleiche(NodeNo, P.m_SourceNo)   )
					{
						return true;
					};



	// GF =    �������� : �
	if (SynRelStr == "��������")
		if (SynFetStr == "�")
			if  ( HasRichPOS (NodeNo, ADJ_SHORT) )
				return true;



	//����������� ����������� ������ ������ ������ ������
	if (SynRelStr == "����_���")
		if (m_Nodes[NodeNo].GetMinWordNo() - m_Nodes[P.m_SourceNo].GetMinWordNo() <=  0  )
			return false;

	long PrepNo = -1;

	if (m_Nodes[NodeNo].m_MainWordNo != -1) 
	{
		const CRusSemWord& SynMainWord = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo];

		// GF =    �������� : �_����
		if (SynRelStr == "��������")
			if (SynFetStr == "�_����")
				if  ( HasRichPOS (NodeNo, ADJ_FULL)  || HasPOS (NodeNo, PARTICIPLE))
					if (GetSynHost (NodeNo)  == -1)
						if ( SynMainWord.HasOneGrammem(rGenitiv) ) 
							return true;

		// GF =    �������� : �_����
		if (SynRelStr == "��������")
			if (SynFetStr == "�_����")
				if  ( HasRichPOS (NodeNo, ADJ_FULL)  || HasPOS (NodeNo, PARTICIPLE))
					if (GetSynHost (NodeNo)  == -1)
						if ( SynMainWord.HasOneGrammem(rInstrumentalis) ) 
							return true;

		if (SynRelStr == "��������")
			if (P.m_pRossDoc->IsCompAdjCortege( C))
				return m_Nodes[NodeNo].HasOneGrammem(rComparative) ;



		/*
		�������, � �� ����, ����� ���������� ������ "� ���� ����"!
		����.
		*/
		if (   m_Nodes[NodeNo].IsPrimitive() 
			&& (SynRelStr == "�_���")
			&& (m_Nodes[NodeNo].m_Words[0].m_Word == "����")
			)
			return false;


		if (   P.m_pRossDoc->IsSynGroup(SynRelNo)
			|| P.m_pRossDoc->IsSynRel(SynRelNo)
			) 
			if ( IsCase(P.m_pRossDoc, SynFet) )
				if (    (m_Nodes[NodeNo].m_MainWordNo != -1)
					&&  !m_Nodes[NodeNo].HasSomePrep() 
					&&  (    !m_Nodes[P.m_SourceNo].HaveCommaAfter() 
					|| !HasRichPOS(P.m_SourceNo, NOUN)
					)
					&&  (     SynMainWord.HasPOS(NOUN)  
					|| SynMainWord.HasPOS(PRONOUN) 
					|| SynMainWord.HasPOS(NUMERAL)  // "����� �� ������� ����� ���� ������� �������������� �������"
					|| SynMainWord.HasPOS(NUMERAL_P)  // "������ ������ ����"
					|| (    SynMainWord.HasPOS(PRONOUN_PREDK)  // ��� ������ ������ ��������
					&& HasRichPOS(P.m_SourceNo, INFINITIVE)			 	
					)
					|| SynMainWord.m_ILE   // "BBS ������ �� ��������" 
					|| (  
					SynMainWord.HasPOS(ADJ_FULL)  
					&& (GetSynRelationsCount(NodeNo)  == 0)
					// � ������������� �������� GetSynRelationsCount (NodeNo)>0,
					// ��� ��� �� ����� ��������� ��������� ����������� 
					//  "�������� ������� ������ ������"
					//  "� ����� ������ ����� ��������� ���������" 
					)

					|| (     m_Nodes[NodeNo].IsPrimitive()
					&&  SynMainWord.IsRusSubstPronounP()   
					&&  !HasIncomingNotWeakSynRelation(NodeNo)
					// "����� �� ����� ���� ������� ��������������"
					// " ������� �� ���������
					)
				 )
				 )
				{
					QWORD Grammems  = m_Nodes[NodeNo].GetGrammems();

					// "����� �����" ( � ���� "�����" �������� - ��, ��, 
					// � � ����� "�����" - ��, ��.)
					// ���� ���������� ������� �� ������� � �����, �� ����� ����� �������� ��������
					// �����
					if (  HasRichPOS (P.m_SourceNo, NUMERAL) ) 
						Grammems =  SynMainWord.GetAllGrammems();
					long  CaseGrammems = GetCaseGrammem(P.m_pRossDoc, SynFet);

					if   (     ((CaseGrammems & Grammems) == CaseGrammems) 
						|| m_Nodes[NodeNo].IsILENode()
						)
				 {

					 SynRealization.m_Grammems = GetCaseGrammem(P.m_pRossDoc, SynFet);
					 return true;
				 }
				}
				else;
			else; 
			if (P.m_pRossDoc->IsLemGroupBegining( SynFet)) 
				if (  m_Nodes[NodeNo].HasSomePrep())
					if (   !P.m_PatternValency.m_IsAddition	 
						|| CheckSemanOfPrep(P.m_PatternValency.m_RelationStr, P.m_PatternValency.m_IsAddition, NodeNo, PrepNo) 
						)
						if (CheckGroupBeginAndCase(SynFetStr, NodeNo, PrepNo))
						{
							if (PrepNo != -1) 
								SynRealization.m_Preps.push_back( m_Nodes[NodeNo].m_SynReal.m_Preps[PrepNo]); 
							return true;
						}
						else;
					else;
				else 
					if (P.m_pRossDoc->ILENo == SynFet)
						return (m_Nodes[NodeNo].m_MainWordNo != -1) && (SynMainWord.m_ILE); 
			// ��� ��������� ����������� �� ����� ��������� ���������� MUA,
			// ������, ��������� MUA ����� ���� ������ � ���������� � � ������ ����������.  

			//GF* = *  ����_��� : ��
			if (SynRelNo == P.m_pRossDoc->ParatAttrSynONo) // ���������� ������ ����, ��� ��� "����_���" 
				if (SynFet !=  P.m_pRossDoc->HyphenNo) // //GF* = *  ����_���:��� ������ ����������� ��������, ��� ��� ����������  ��������� � ���� ����� 
					// ��������� ���� "����-��������", "�������-�������"
					if  (		m_pData->GetRusGramTab()->GleicheCaseNumber(m_Nodes[P.m_SourceNo].m_GramCodes.c_str(), m_Nodes[NodeNo].m_GramCodes.c_str()) 
							||	SynMainWord.m_ILE
						)
					{
						return true;
					};

	};

	//GF* = *  ����_��

	PrepNo = -1;

	if (SynRelStr == "����_��" ) 
		if ( SynFetStr == "���" )
			return HasRichPOS(NodeNo,ADV);
		else
			if (SynFet == -1)
				if (   (    m_Nodes[NodeNo].HasSomePrep()
					&&  CheckSemanOfPrep(P.m_PatternValency.m_RelationStr, P.m_PatternValency.m_IsAddition, NodeNo, PrepNo)  
					)
					|| (   (m_Nodes[NodeNo].m_MainWordNo != -1)
					&& m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].HasPOS(ADV)
					&& (P.m_PatternValency.m_RelationStr != "TIME")			&& !IsLocSemRel (P.m_PatternValency.m_RelationStr)
					)
					|| (		IsTimeAdverbial(NodeNo)  
					&&	(P.m_PatternValency.m_RelationStr == "TIME")		   )
					|| (		(m_Nodes[NodeNo].GetType() == LocRoss)
					&&	(GetRossHolder(LocRoss)->HasCX(m_Nodes[NodeNo].GetUnitNo(), P.m_PatternValency.m_RelationStr, "D_SEM_REL")) 
					)
					|| (		(P.m_PatternValency.m_RelationStr == "LOK")			
					&&		IsLocNode(NodeNo) 
					&&		(	(		 m_Nodes[NodeNo].HasSomePrep()
					&&	CheckSemanOfPrep(P.m_PatternValency.m_RelationStr, P.m_PatternValency.m_IsAddition, NodeNo, PrepNo)  
					)
					||	HasRichPOS (NodeNo, ADV)
					)

					)

					/*|| (     (m_Nodes[NodeNo].GetType() == OborRoss)
					&&  m_pData->Oborottos[m_Nodes[NodeNo].GetUnitNo()].m_bRusOborAdverbial
					)*/
					)
				{
					if (PrepNo != -1) 
						SynRealization.m_Preps.push_back( m_Nodes[NodeNo].m_SynReal.m_Preps[PrepNo]); 
					return true;
				};

	// GF* = *  �_��� : ���
	// GF*     = *  �_��� : ���
	if (   (SynRelNo == P.m_pRossDoc->DirectObjSynONo) 
		|| (SynRelNo == P.m_pRossDoc->IndirObjSynONo) 
		|| (SynRelStr == "��������")
		)
		if (SynFet == P.m_pRossDoc->InfinitiveNo) 
		{
			// ��������������� �� ����� ��������� �����������, ������� ����� �� ����
			// �������� "������� ������� �������������� �������"
			// �����  "�������" ���������� ������������ ����� "�������"
			if (    HasRichPOS(P.m_SourceNo, NOUN) 
				&& (m_Nodes[NodeNo].GetMinWordNo() < m_Nodes[P.m_SourceNo].GetMinWordNo())
				)
				return false;

			return HasRichPOS(NodeNo, INFINITIVE);
		};

	if (		(P.m_PatternValency.m_RelationStr == "IDENT")  	   
		&& (   (isdigit((BYTE) m_Nodes[NodeNo].m_Words[0].m_Word[0]))
		|| (m_Nodes[NodeNo].m_Words[0].m_ILE)
		|| (m_Nodes[NodeNo].m_bQuoteMarks)
		|| (    (m_Nodes[NodeNo].m_SynGroupTypeStr == NOUN_NUMERAL)
		&& (m_Nodes[NodeNo].m_Words.size() > 0)
		&& (m_Nodes[NodeNo].m_Words[0].m_Word == "N")
		)
		)
		)
		return true;

	if (SynRelStr == "*" ) 
		return true;

	return false;
};



bool CRusSemStructure::HaveOnlyPunctBetween (size_t NodeNo1, size_t NodeNo2, int punct_char) const
{
  if ( m_Nodes[NodeNo1].GetMaxWordNo() > m_Nodes[NodeNo2].GetMaxWordNo() )
	  swap (NodeNo1, NodeNo2);

  // ���������, ���� � ������ ���������� ����� ������ ������ ���� ���� �������, �� ���������� ������,
  // ���� ����� ������ ������ ������ ����������� ���� (�� �������� �����) ��� ����, ������� ��������� 
  // �� ����� NodeNo
  if  (     (m_Nodes[NodeNo1].GetMaxWordNo()+1  == m_Nodes[NodeNo2].GetMinWordNo())
		 && (m_Nodes[NodeNo1].HasPostPunct(punct_char))
	  )
  {
	  for (long i=NodeNo1+1;  i < NodeNo2;i++)
		 if (IsBetween(m_Nodes[i], m_Nodes[NodeNo1], m_Nodes[NodeNo2]))
		  if (!m_Nodes[i].IsAbstract())
			   return false;
      return true;
  }
  else
  if (m_Nodes[NodeNo1].GetMaxWordNo()+2  == m_Nodes[NodeNo2].GetMinWordNo() )
  {
	  // ���� ������ ���� ����� ������ NodeNo1 � NodeNo2, ��������� ������ �� ������ ����� ����� ����������,
	  // �� ������� ��� ������ (����� ����������� ���� �������� � ���������� �������, ������� �������� 
      // ���������).
	  long Count_Punct = 0;
	  for (long i=NodeNo1+1;  i < NodeNo2; i++)
		if ( IsBetween(m_Nodes[i], m_Nodes[NodeNo1], m_Nodes[NodeNo2]) )
         if (      m_Nodes[i].IsPrimitive() 
			   && (m_Nodes[i].m_Words[0].m_Word[0] == punct_char)
			)
		 	 Count_Punct ++;
		 else
		   if (!m_Nodes[i].IsAbstract())
		 	    return false;

      return Count_Punct == 1;
  };
  return false;
};

bool CRusSemStructure::HasCommaBefore (size_t NodeNo) const
{
  long nd = FindLeftClosestNode(NodeNo);
  if (nd == -1) return false;
  if (m_Nodes[nd].IsWordContainer()) 
	  if (   (m_Nodes[nd].m_Words[0].m_Word == ",")
		   || m_Nodes[nd].HasPostPunct(',') 
		 )
		  return true;

  return false;
};


bool CRusSemStructure::HaveNounNodeBetweenInOneClause (size_t NodeNo1, size_t NodeNo2) const
{
  assert (m_Nodes[NodeNo1].m_ClauseNo == m_Nodes[NodeNo2].m_ClauseNo);

  if ( NodeNo1 > NodeNo2 )
	  swap (NodeNo1, NodeNo2);

  for (long i=NodeNo1+1; i <  NodeNo2 ; i++)
	  if  ( HasRichPOS(i,NOUN) )
		  return true;

  return false;
};



void CRusSemStructure::InitGrammarMatrix1 (long ClauseNo)
{
	CRusSemClause& C = m_Clauses[ClauseNo];

	// �������� �������������� �������
	long NodesCount = C.GetNodesCount();
	if (NodesCount == 0) return;
	/*
	����� ������������������� ��� �������, ������ ���
	�����  ��������� ����� ���� (ModalCopul). �����, ����� 
	��� ��� ��� ����� ���� ����� ����.
	*/
	C.InitGramMatrix();


	long NodeNo=C.m_BeginNodeNo;
	for (; NodeNo < C.m_EndNodeNo; NodeNo++)
		if (!m_Nodes[NodeNo].IsWordContainer())
			break;
	/*
	EndWordContNodeNo - ����� ����, ������� ��������� �� ��������� 
	�����, ��������� ����� (IsWordContainer)
	������� ����� ���������������� ������ ���  WordContainer.
	*/
	long EndWordContNodeNo = NodeNo;

	// �������������� ����  HaveOnlyCommaBetween
	for (long i=C.m_BeginNodeNo; i < EndWordContNodeNo-1; i++)
		if (m_Nodes[i].IsWordContainer())
		{
			long k = -1;

			if (HaveOnlyPunctBetween(i, i+1, ','))
				k = i+1;

			if (i+2 < EndWordContNodeNo)
				if (HaveOnlyPunctBetween(i, i+2, ','))
					k = i+2;

			if ( k != -1)
			{
				C.AddFlagToGramMatrix(i,k, HaveOnlyCommaBetween);

				// �������������� ����  HaveCommaBetween
				// ���� ���� ����������� �� ����� HaveOnlyCommaBetween
				for (long j=k; j < EndWordContNodeNo; j++)
					for (long l=i; l>=C.m_BeginNodeNo; l--)
					{
						C.AddFlagToGramMatrix(j,l, HaveCommaBetween);
					}
			};

		};




	// �������������� ����� HaveFiniteVerbBetween � HaveOnlyConjBetween
	vector<bool> StrongRoots(m_Nodes.size(), false);
	for (long i=C.m_BeginNodeNo; i < EndWordContNodeNo; i++)
		StrongRoots[i] =		(HasRichPOS(i, VERB) || HasPOS(i, ADJ_SHORT) || HasPOS(i, PARTICIPLE_SHORT))
							&&	!ContainsSemCopul(i);
		// ������� ���� - ��� �������� ���������, ������� �� ����� ��������� ���� �������,
		// ���� ��������� ������� ��� ������, ����� ����� "��� ��� ���� ��������" �� ����� ����������
	    
							

	for (long i=C.m_BeginNodeNo; i < EndWordContNodeNo; i++)
		if (!HasRichPOS(i, INFINITIVE))
			for (size_t k=i+2; k < EndWordContNodeNo; k++)
				if (!HasRichPOS(k, INFINITIVE))
				{

					// �������������� ���� ������� ����� ����� ������� ������� � ������ �����, 
					// �������� ��������������� ��� �������� ��������� (����������� �������)
					for (long l=i+1; l < k; l++)
						if  ( StrongRoots[l] )
						{
							C.AddFlagToGramMatrix(i,k, HaveFiniteVerbBetween);
							break;
						}

						// �������������� ���� ������� ������ ������ �����
						if (k == i+2)
							if ( m_Nodes[i+1].IsLemmaList("�", "���", "��", "�", "") )
							{
								C.AddFlagToGramMatrix(i,k, HaveOnlyConjBetween);
							};
				};

	for (long i=C.m_BeginNodeNo; i < EndWordContNodeNo; i++)
		for (size_t k=i+1; k < EndWordContNodeNo; k++)
		{
			if (AreOldNodesOfTheSameCase(i, k))
			{
				C.AddFlagToGramMatrix(i,k, OldNodesOfTheSameCase);
			}
		}
};

void CRusSemStructure::InitGrammarMatrix2 (long ClauseNo)
{
  CRusSemClause& C = m_Clauses[ClauseNo];

  long i=C.m_BeginNodeNo;
  for (; i < C.m_EndNodeNo; i++)
	  if (!m_Nodes[i].IsWordContainer())
		  break;
   /*
		EndWordContNodeNo - ����� ����, ������� ��������� �� ��������� 
		�����, ��������� ����� (IsWordContainer)
		������� ����� ���������������� ������ ���  WordContainer.
   */
  long EndWordContNodeNo = i;

  for (i=C.m_BeginNodeNo; i < EndWordContNodeNo; i++) 
     for (size_t k=i+1; k < EndWordContNodeNo; k++)
		 if (AreOldNodesOfTheSameCase(i, k))
		 {
			 C.AddFlagToGramMatrix(i,k, OldNodesOfTheSameCase);
		 }
};








bool CRusSemStructure::IsPattern (CSemPattern& P, size_t NodeNo, string& SyntacticRelation, CSynRealization& SynRealization)
{
	SynRealization.SetEmpty();
	SyntacticRelation = "";
	//if (P.m_SourceNo != -1)

    if (P.m_ObligLexFets.size() > 0)
	{
		long i=0;
		for (; i <P.m_ObligLexFets.size(); i++)
			if (P.m_ObligLexFets[i][0] == '#')
			{
				if (_find(m_Nodes[NodeNo].m_HigherConcepts, P.m_ObligLexFets[i]))
					break;
			}
			else
			{
			  if (m_Nodes[NodeNo].IsLemma(P.m_ObligLexFets[i]))
				  break;
			};

		if (i == P.m_ObligLexFets.size()) return false;
	};
	


	if (P.m_GramCorteges.size() == 0) return false;

    if (P.m_GramCorteges[0].m_DomItemNos[0] == -1) return false;


	for (SynRealization.m_CortegeNo =  0; SynRealization.m_CortegeNo < P.m_GramCorteges.size(); SynRealization.m_CortegeNo++)
      if (   CheckPatternGramFetLine(P, SynRealization, NodeNo)
		  
		 ) 
	  {
         
		 if (P.m_pRossDoc->IsSynRel (P.GetSynRel(SynRealization.m_CortegeNo)) )
			 SyntacticRelation = P.GetSynRelStr(SynRealization.m_CortegeNo);
		  SynRealization.m_Cortege = P.m_GramCorteges[SynRealization.m_CortegeNo];
		 return true;
	  }
	 SynRealization.m_CortegeNo= - 1;
	
	 return false;

};

bool CRusSemStructure::AreOldNodesOfTheSameCase (long NodeNo1, long NodeNo2) const 
{
	if  (     (m_Nodes[NodeNo1].m_MemNodeNo == -1)
	       || (m_Nodes[NodeNo2].m_MemNodeNo == -1)
	    )
		return false;
	assert (m_Nodes[NodeNo1].m_MemNodeNo < m_MemNodes.size() );
	const CMemNode& N1 = m_MemNodes[m_Nodes[NodeNo1].m_MemNodeNo];

	assert (m_Nodes[NodeNo2].m_MemNodeNo < m_MemNodes.size() );
	const CMemNode& N2 = m_MemNodes[m_Nodes[NodeNo2].m_MemNodeNo];
	
	long s[10000];
	assert (N1.m_Cases.size() + N2.m_Cases.size() < 10000);
	long* Out = set_intersection(
		N1.m_Cases.begin(), N1.m_Cases.end(), 
		N2.m_Cases.begin(), N2.m_Cases.end(),
		s); 
	return Out > s;
};

/*
 � ���� ������� NodeNo ������ ���������� �� P.m_SourceNo.
 ��������, � ������� ������, ����� NodeNo - ��� ����� 
 ������, � P.m_SourceNo - ����� ������, �� ������� ����
 ����� ������
*/
void CRusSemStructure::TryPatternOnNodes(size_t NodeNo, size_t ClauseNo, CSemPattern& P)
{
	string  SyntacticRelation;
	CSynRealization SyntacticRealization;
	const CRusSemClause& C = m_Clauses[ClauseNo];

	
#if Trace == 1
	P.TracePattern();
#endif


	for (size_t i=0; i<m_Nodes.size(); i++)
		if (     (i != NodeNo)
			&&  IsInClause(i, ClauseNo)
			&&  m_Nodes[i].IsWordContainer()
			/*  ��� ��� ����  ������ ����������� ������ ���
			*/
			&& !(C.GetGramMatrix(NodeNo,i) & OldNodesOfTheSameCase)
			)
		{
			StartTimer("TryPatternOnNodes",2);
#if Trace == 1
			rml_TRACE (GetNodeStr(NodeNo).c_str());
			rml_TRACE (" <--?--> ");
			rml_TRACE (GetNodeStr(i).c_str());
#endif

			bool Result = false;


			if (   
					!(C.GetGramMatrix(P.m_SourceNo,i) & HaveFiniteVerbBetween)
				&&	!(C.GetGramMatrix(P.m_SourceNo,i) & HaveOnlyConjBetween)
				&&	IsPattern (P, i, SyntacticRelation,  SyntacticRealization) 
				)
			{

				if (SyntacticRelation == "X!")
				{
					AddRelation(CRusSemRelation(P.m_PatternValency, i, NodeNo, SyntacticRelation));
					m_Relations.back().m_bReverseRel = true;
				}
				else
					AddRelation(CRusSemRelation(P.m_PatternValency, NodeNo, i, SyntacticRelation));

				CRusSemRelation&  R  = m_Relations.back();
				R.m_CannotHaveOnlyCommaBetween = true;

				// ������ ��������� ��������������
				if (SyntacticRelation == "�����")
					if (HasRichPOS (i, ADJ_FULL))
						R.m_CannotHaveOnlyCommaBetween = false;

				R.m_SynReal =  SyntacticRealization;
				
				R.m_SemFets = GetIndexedSemFets(P, true, false);
				/*  �������� � ����-������� ������������ ������ ������*/
				if	(    m_Nodes[i].IsMainTimeRossNode() 
					&& (P.m_PatternValency.m_RelationStr == "TIME") // ���� ����-������ ��������� ����������� �� TIME, ������ ��� ���������			 
					)
					R.m_SynReal.m_Preps.clear();

				R.m_LexFets = P.m_LexFets;
				R.m_LexFets.insert(R.m_LexFets.begin(), P.m_ObligLexFets.begin(), P.m_ObligLexFets.end());

				Result = true;
			}

			EndTimer("TryPatternOnNodes");
#if Trace == 1
			rml_TRACE (" => %s\n", Result ? "success" : "failed");
#endif
		};


};

bool HasCopulPattern (const CSemPattern& P)
{
	for (long i=0; i < P.m_GramCorteges.size();i++)
		if (P.m_GramCorteges[i].m_DomItemNos[0] == P.m_pRossDoc->CopulNo)
			return true;

    return false;
};

bool HasModalCopulPattern (const CSemPattern& P)
{
	for (long i=0; i < P.m_GramCorteges.size();i++)
		if (P.m_GramCorteges[i].m_DomItemNos[0] == P.m_pRossDoc->ModalCopulNo)
			return true;

    return false;
};

/*
 ���������� ������ ���� ������� �� �������. 
 "� ������� ������". ����� ���� ������� LF �� 
 "������" � "��������" �� ��������� Oper1. ���
 ������� ������������, ��� ������ ����������� ����� "������"
 ����� �������� ���������� ��� �������, � ��� ��������,
 ��� ��� �� ����� ���� �������� ��� ��������������� (��������,
 � ������� ������������ ������). ������ �������,
 "�� ������� ������ ���" - ����� ��� ���� ��������� �����������.
 ������� �� ����� "� ������� ������" ������ �����������
 "������" ��������.
*/
bool ValencyIsOccupiedByLexFunct(const CRusSemStructure& R, size_t NodeNo, size_t ValencyNo)
{
	int ValNo;	
	for (long i=0;i < R.m_LexFuncts.size(); i++)
	 if (R.m_LexFuncts[i].m_SourceNodeNo == NodeNo)
	   if (IsOper(R.m_LexFuncts[i].m_LexFunctName, ValNo))
		   for (long j=0; j<R.m_Nodes[NodeNo].m_NotProhibitedValSets.size(); j++)
			   if  (     ( R.m_Nodes[NodeNo].m_NotProhibitedValSets.size() > ValNo-1)
				     &&  ( R.m_Nodes[NodeNo].m_NotProhibitedValSets[j][ValNo-1] ==  ValencyNo)
				   )
				   return true;

	return false;	
};



void CRusSemStructure::FindActants(size_t NodeNo)
{
	CSemPattern P;
	size_t ClauseNo = m_Nodes[NodeNo].m_ClauseNo;
#if Trace == 1
	rml_TRACE ("\n");
	rml_TRACE (GetNodeStr(NodeNo).c_str());
	rml_TRACE ("\n");
#endif

	for (size_t ValencyNo = 0; ValencyNo < m_Nodes[NodeNo].m_Vals.size(); ValencyNo++ )
		if (!ValencyIsOccupiedByLexFunct(*this, NodeNo, ValencyNo))
		{
			GetActantPattern (NodeNo, ValencyNo, P);

			if  ( HasCopulPattern(P) )    
			{
				CSemPattern CopulPattern = P;
				CRusSemNode NewCopulNode = CreateCopulNode(ClauseNo);
				long CopulNodeNo = InsertNode(NewCopulNode);
				assert (m_Clauses[ClauseNo].m_EndNodeNo - 1 == CopulNodeNo);
				m_Clauses[ClauseNo].UpdateSizeOfGramMatrix();
				AddRelation(CRusSemRelation(P.m_PatternValency, NodeNo, CopulNodeNo,  ""));

				bool PassiveCase =    m_Nodes[NodeNo].IsPassiveVerb() && (ValencyNo == 0);
				size_t k = PassiveCase ? 1 : ValencyNo;
				m_Nodes[CopulNodeNo].m_Vals.clear();
				m_Nodes[CopulNodeNo].m_Vals.push_back (CValency("F-ACT", A_C, GetRossHolder(Ross), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1, m_Nodes[NodeNo].GetUnitNo()));
				m_Nodes[CopulNodeNo].m_Vals.push_back (CValency("S-ACT", A_C, GetRossHolder(Ross), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2, m_Nodes[NodeNo].GetUnitNo()));	

				// ������ F-ACT		
				CopulPattern.m_GramCorteges.clear();
				if  (PassiveCase)
				{
					LoadFromDictForPassive(m_Nodes[NodeNo].GetUnitNo(), 1, 1, CopulPattern);
					CopulPattern.m_PatternValency = CValency ("F-ACT",A_C,GetRossHolder(Ross), 1, 1, m_Nodes[NodeNo].GetUnitNo());
					if (CopulPattern.m_GramCorteges.size() == 0) CopulPattern.m_GramCorteges.push_back(GetSubjCortege(GetRossHolder(Ross)));
					TryPatternOnNodes(CopulNodeNo, ClauseNo, CopulPattern);
				}
				else
				{
					CopulPattern.InitSemPattern(GetRossHolder(Ross), m_Nodes[NodeNo].GetUnitNo(), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1);
					CopulPattern.LoadSemFromDict();
					if (CopulPattern.LoadGramFromDict())
					{
						CopulPattern.m_PatternValency = CValency ("F-ACT",A_C, GetRossHolder(Ross), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1, m_Nodes[NodeNo].GetUnitNo());			
						TryPatternOnNodes(CopulNodeNo, ClauseNo, CopulPattern);
					};
				};

				// ������ S-ACT		
				CopulPattern.m_GramCorteges.clear();
				CopulPattern.InitSemPattern(GetRossHolder(Ross), m_Nodes[NodeNo].GetUnitNo(), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2);
				CopulPattern.LoadSemFromDict();
				if (CopulPattern.LoadGramFromDict())
				{
					CopulPattern.m_PatternValency = CValency ( "S-ACT", A_C, GetRossHolder(Ross), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2, m_Nodes[NodeNo].GetUnitNo());			
					TryPatternOnNodes(CopulNodeNo, ClauseNo, CopulPattern);
				};
			}

			if ( HasModalCopulPattern (P) )
			{
				CSemPattern CopulPattern = P;

				CRusSemNode NewCopulNode = CreateModalCopulNode(ClauseNo);
				CopulPattern.m_SourceNo = InsertNode(NewCopulNode);
				assert (m_Clauses[ClauseNo].m_EndNodeNo - 1 == CopulPattern.m_SourceNo);
				m_Clauses[ClauseNo].UpdateSizeOfGramMatrix();

				// ���������� ��������������� ���������
				m_DopRelations.push_back(CRusSemRelation(CValency("ModalCopul", A_C, GetRossHolder(Ross)),  NodeNo,  CopulPattern.m_SourceNo,  "ModalCopul"));     

				m_Nodes[CopulPattern.m_SourceNo].m_Vals.clear();

				m_Nodes[CopulPattern.m_SourceNo].m_Vals.push_back (CValency("SUB", A_C, GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1, m_Nodes[NodeNo].GetUnitNo()));
				m_Nodes[CopulPattern.m_SourceNo].m_Vals.push_back (CValency("CONTEN", A_C, GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2, m_Nodes[NodeNo].GetUnitNo()));		
				CopulPattern.m_GramCorteges.clear();

				CopulPattern.InitSemPattern(GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].GetUnitNo(),m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1);
				CopulPattern.LoadSemFromDict();
				if (CopulPattern.LoadGramFromDict())
				{
					CopulPattern.m_PatternValency = m_Nodes[CopulPattern.m_SourceNo].m_Vals[0];
					TryPatternOnNodes(CopulPattern.m_SourceNo, ClauseNo, CopulPattern);
				};

				CopulPattern.m_GramCorteges.clear();
				CopulPattern.InitSemPattern(GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].GetUnitNo(),m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2);
				CopulPattern.LoadSemFromDict();
				if (CopulPattern.LoadGramFromDict())
				{
					CopulPattern.m_PatternValency = m_Nodes[CopulPattern.m_SourceNo].m_Vals[1];
					TryPatternOnNodes(CopulPattern.m_SourceNo, ClauseNo, CopulPattern);
				};


			}

			TryPatternOnNodes(NodeNo, ClauseNo, P);
		};

};



void CRusSemStructure::FindReverseActantForPreps(size_t ClauseNo)
{
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	  if (    m_Nodes[NodeNo].HasSomePrep()
		   && (GetIncomingRelationsCount (NodeNo, false) == 0)
		   && IsInClause (NodeNo, ClauseNo)
		  )
	  {
		  CSemPattern P;
		  P.m_SourceNo = NodeNo;
		  P.m_pRossDoc = GetRossHolder(OborRoss);
		  WORD PrepUnitNo = m_Nodes[NodeNo].m_SynReal.m_Preps[0].m_UnitNo;
		  // ��������� ������ ����������� �������� - ��� ��������� �����������
		  P.InitSemPattern(P.m_pRossDoc, PrepUnitNo, 1, 0);
		  P.LoadGramFromDict();
		// ����� �������� X!, ������ ��� ������� �������� CAT=RELAT		  
		  P.InsertReverseSynOToTheBegining();
		  // �������� ������ ����������� �������� (�� ���������, ������� ��� ��������)
		  StringVector Vals;
		  P.m_pRossDoc->GetSimpleFieldItemsFromArticle (PrepUnitNo, "VAL", 0, 0, Vals);
		  if (Vals.size() > 0)
		     P.m_PatternValency.m_RelationStr = Vals[0];
		  P.m_PatternValency.m_bOptional = true;
		  long OldRelsCount = m_Relations.size();
		  /*
		    ����� �������� ������ ������������ � ��������������,
			�����  ��������� ������ "��� �� ����� ��������"
		  */
		  bool bsave = m_Nodes[NodeNo].m_bFullGleiche;
		  m_Nodes[NodeNo].m_bFullGleiche = false;
		  TryPatternOnNodes(NodeNo, ClauseNo, P);
		  m_Nodes[NodeNo].m_bFullGleiche = bsave;


  		  // ����� �������, ��� ��� ��������� ������������� �� �� 
		  // �������, �������  P.m_pRossDoc = 0;
		  // � ��������� ������, ������ �������� �������, � ������� ���� �����������
		  // ������ �� ��� ���������� ������ �� ������ ������� ������.
		  // ��������, "�� ���� �� ����" ����� ����� ���������� ���� 1, � ������
		  // ���� 3, � �������� ��������� ����������� �� "��+�"

		  
		  // ��������� ���� ������ ��� ����������� ���������� �������
		  for (long i=OldRelsCount; i <m_Relations.size();i++)
		  {
			  m_Relations[i].m_SynReal.m_Preps.push_back (m_Nodes[NodeNo].m_SynReal.m_Preps[0]);
			  m_Relations[i].m_Valency.m_RossHolder = 0;
		  };


	  };

};

/*
�����:
���������� ���������� ������ �� ����������������
��� ����� ����� �� � ��������, �������������� ����� ������
��������:
������ �� �� ���������������

�������:
��� ����� � ������ ����� ������ ����������.
������� �� ����� ��������� ��������.
����� ������ ������� ���������� � ����������� � ����.
��� � ��� ��� �����, ������������� � ��������� ���� �������� ��
���� ��������� ���������
������� �������� ����� ��� ������� ���� �������
*/

void CRusSemStructure::HeuristicsLocative(long ClauseNo) 
{
  UpdateBlockedRelations();
  for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  NodeNo < m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
  if (    m_Nodes[NodeNo].IsWordContainer()
	  &&  m_Nodes[NodeNo].HasSomePrep()
	 )
  {
	  long NounNodeNo = -1;
	  long VerbNodeNo = -1;
	  for (long i=0; i<m_Nodes[NodeNo].m_InRels.size();i++)
			if	(		(m_Relations[m_Nodes[NodeNo].m_InRels[i]].m_Valency.m_RelationStr == "LOK")		   
					&&	m_Relations[m_Nodes[NodeNo].m_InRels[i]].m_bReverseRel
				)
				{
					long N = m_Relations[m_Nodes[NodeNo].m_InRels[i]].m_SourceNodeNo;
					if (HasRichPOS(m_Nodes[N], NOUN))
						if (m_Nodes[N].GetMinWordNo() < m_Nodes[NodeNo].GetMinWordNo())
							NounNodeNo = N;

					if (HasRichPOS(m_Nodes[N], VERB))
						if (m_Nodes[N].GetMinWordNo() > m_Nodes[NodeNo].GetMinWordNo())
							VerbNodeNo = N;
				};

	  if (  (NounNodeNo != -1) && (VerbNodeNo != -1))
	  {
		   vector<long> Rels;
		   FindRelations (VerbNodeNo, NodeNo, Rels);
		   DeleteRelations(Rels);
		   UpdateBlockedRelations();
	  };

  };
};

/*
������������ ������, ����� "������" �� ����������� �������� ���������, ��������: 
  ������ ��������� ������� ������, � ����������� � ���� �����!
  ����� �������� - ������ ������ �� ������.
  - ������ ��� ����������.
  ������� ���� ������.	
� ��������, � ������ "��� �� � ��� �����������" ��� ��������� �� ����� ��������,
��������� ����� "������" ������������� "�����������" �� ����������� .
*/
void CRusSemStructure::DealUnboundModalCopul(long ClauseNo) 
{
	for (long i=0; i < m_Nodes.size();i++)
		if (   IsInClause(i, ClauseNo)
			&& (m_Nodes[i].m_NodeType == ModalCopul)
		   )
		{
			vector<long> Rels;
			GetIncomingDopRelations(i, Rels);
			assert (Rels.size() == 1);
			if (GetIncomingRelationsCount(m_DopRelations[Rels[0]].m_SourceNodeNo, false) > 0)
				continue;
			AddRelation(CRusSemRelation(CValency("ModalCopul",A_C), i, m_DopRelations[Rels[0]].m_SourceNodeNo, ""));
			m_DopRelations.erase(m_DopRelations.begin() + Rels[0]);
		};
};


//#pragma optimize( "", on )
