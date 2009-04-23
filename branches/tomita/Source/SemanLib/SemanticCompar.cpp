#include "stdafx.h"
#include "SemanticRusStructure.h"

/*
������� ��� �������� ������
����� �������� ��������������� SF = 1 SUBST 2 ABSTR 3 PARAM 4 ESTIM 5 INTEL ��������������. 1) ���� �� ���������  ������/������ + � �� �� � � �� �������� �������������, 
�����  ����� ������������� ������� ���������� �� ��������������� "�������", � �� �� ������������
"�����":
��������:
  ��� ������ ����  
  ���� ������ �������. 
2) ���� �� ��������� ������ "���(���) ������" ��� "���(���) ������" + � ��  
(���������� ����� ����������� ����� � ������� ������� "more and more" ("less and less"))
  +��� ������ ������ ���������� 
  + ��� ����������� ����������� ��� ������ � ������ ����� (� ������������� )
3) ���� ������ �� �������������� ����� ���� ������������ 
            +������ ���� ����� �����  ������ �� ������

*/

bool CRusSemStructure::FindComparWithNoActantAndCheck(long ClauseNo)
{
 /*
  ������ �������: ����� ���� "������" ��� "������" ��� ������������ �� ����� ������
  ����������� ��������������� � ������������ �����, ������������� 
  ������������  �� SF.
   ����� ������� �� ����� "���� ������ �����" �� ����������    
   ������������� ����� ������ ��� �������������.

 */
  vector<bool> UncounNodes;
  UncounNodes.resize(m_Nodes.size());
  for (long i=0; i < m_Nodes.size(); i++)
	 if (IsInClause(i, ClauseNo))
		UncounNodes[i] = 
				HasRichPOS(i, NOUN)  
			&& m_Nodes[i].HasOneGrammem (rSingular)
			&& (   HasSemFetPro(m_Nodes[i].m_NodeSemFets, "SUBST") // "����"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "ABSTR") // "�����������"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "PARAM") // "��������"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "ESTIM") // "�������"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "INTEL") // "��"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "EMOT") // "�������"			 
				);


  for (long i=0; i+1 < m_Nodes.size(); i++)
	  if (IsInClause(i, ClauseNo))
		  if (   HasRichPOS(i, NUMERAL) 
			  && m_Nodes[i].HasOneGrammem (rComparative)
			 )
		  {
			  if  ( m_Nodes[i+1].HasOneGrammem (rPlural) ) continue;
			  if  (UncounNodes[i+1]) continue;

			  /*
				�� ��� ������ ��������, ��� ��������.
			  */
			  if  ( !m_Nodes[i+1].HasOneGrammem (rGenitiv)) continue;
			  return false;
		  };
 /*
  ����� ������� �������
 */

 /* 
 ������  �������: ������������� ������� �� ��������������� "�������"- "big"
 ("���������" - "small") ������������ ������ � ������������� ������� ���� "���� ������ �������" 

 � �� ����� "�� �������� ������ ����" ����� "������" - ��� ������� "�����".
*/

 if (m_Clauses[ClauseNo].m_ClauseType != COMPARATIVE_T)
 	 for (long i = m_Clauses[ClauseNo].m_BeginNodeNo+1;  i < m_Clauses[ClauseNo].m_EndNodeNo ; i++)
		 if (    m_Nodes[i].IsLemmaList ("�������", "���������", "") 
		    &&	m_Nodes[i].HasOneGrammem(rComparative)
		 )
	  return false;



 /*
  ����� ������� �������
 */


 /*
  ���� ����� ������������� ������������ ����� "���", ����� ��������� 
  �������� "�������" ����� �������������, � ����  "���" �������
 */
  for (long i = m_Clauses[ClauseNo].m_BeginNodeNo+1;  i < m_Clauses[ClauseNo].m_EndNodeNo ; i++)
  	  if (			HasRichPOS(i, NUMERAL) 
			  &&	m_Nodes[i].HasOneGrammem (rComparative)
			  &&	CanBeDeleted(i)
		 )
	  {
			  if ( !m_Nodes[i-1].IsWordForm("���") ) continue;
	  		  m_Nodes[i].m_RelOperators.push_back("�������");
			  DelNode(i-1);
			  i--;
	  }

  return true;
};



// �� ��� ������ ��������, ��� ��������.
// ������� �����  ������ ...,���
// � �� ����  �������� ������, ��� ����
/// === ������������ ������ ��������� ���������� � ������ 
void CRusSemStructure::BuildOutcomingOf_CHEM_Obor(long ClauseNo)
{
 try {
  SetNodeToDeleteFalse();
  int EndNodeNo = m_Clauses[ClauseNo].m_EndNodeNo;
  
  long CompNodeNo = EndNodeNo-1;
  for (;  CompNodeNo >=  m_Clauses[ClauseNo].m_BeginNodeNo; CompNodeNo--)
    if ( m_Nodes[CompNodeNo].m_bCompAdj)
	{
		
		long  k=CompNodeNo+1;
		for (; k < EndNodeNo; k++)
        if  (     m_Nodes[k].IsPrimitive()
			   && (m_Nodes[k].m_Words[0].m_Lemma == "���")
			 )
		break;
        if (k == EndNodeNo) continue;
        long ChemNodeNo = k;
		//�������� SF1 ����������� �� ����		
		vector<QWORD> SemFets = GetSemFetsOfFirstValency(CompNodeNo);

		for (k=ChemNodeNo+1; k < EndNodeNo; k++)
           if (     IsAdverbial(k)
				 || HasRichPOS (k, NOUN) 
			     || HasRichPOS (k, PRONOUN)
			  )
		   {
			 // ������� ���������, ������� ���� ��������� �� �����������
			 long RelNo = FindFirstRelation(CompNodeNo, k);
			 if (RelNo != -1)
				 EraseRelation(RelNo);
			 AddRelation(CRusSemRelation(CValency(), CompNodeNo, k, ""));
			 m_Relations[m_Relations.size() - 1].m_SemFets = SemFets;
			 break;
		   };
		if (k == EndNodeNo) continue;
		if (!CanBeDeleted(ChemNodeNo)) continue;
		long AfterChemNodeNo = k;




		for (k=m_Clauses[ClauseNo].m_BeginNodeNo; k < EndNodeNo; k++)
         if ( k!= ChemNodeNo)
		  if ( k!= CompNodeNo )
			if ( k!= AfterChemNodeNo )
           if (    (    IsAdverbial(AfterChemNodeNo)
			         && IsAdverbial(k)
				   )
				 ||(
				        !IsAdverbial(AfterChemNodeNo)
					 && (    HasRichPOS (k, NOUN) 
						  || HasRichPOS (k, PRONOUN)
						)
				   )
			  )
		   {
			 AddRelation(CRusSemRelation(CValency(), CompNodeNo, k, ""));
			 m_Relations[m_Relations.size() - 1].m_SemFets = SemFets;
		   };
		
          
         

		// ��������� "���"
        m_Nodes[ChemNodeNo].m_bToDelete = true;

		// ������ ���������� ���������� ������������
		m_Nodes[CompNodeNo].m_NodeType = MNA;
		m_Nodes[CompNodeNo].m_MNAType = CHEMOborot;
		
		break;

	};

  DelNodesToDelete();
  // ������������ ������ "������ ����� � ��� ���� ������, ��� � ���� ����"

  if (m_Clauses[ClauseNo].m_ClauseType == COMPARATIVE_T)
  if (m_Clauses[ClauseNo].m_bHasCHEM)
  if (CompNodeNo != -1)
  {
    for (long i = m_Clauses[ClauseNo].m_BeginNodeNo;  i < m_Clauses[ClauseNo].m_EndNodeNo ; i++)
	 if (    (GetIncomingRelationsCount (i) == 0)
		  && (   (HasRichPOS (i, NOUN))
		      || (HasRichPOS (i, PRONOUN))
			 )
		  && m_Nodes[i].HasOneGrammem(rNominativ)
	    )
	 {
		 if(!m_Nodes[CompNodeNo].m_Vals.empty())
			AddRelation(CRusSemRelation(m_Nodes[CompNodeNo].m_Vals[0], i, CompNodeNo, ""));
	 };
  };


 }
 catch (...)
 {
	 ErrorMessage ("BuildOutcomingOf_CHEM_Obor Failed");
	 throw;
 };

};



/*
  ������ �� ������������� "������" ("������") � � "���":
 "�� ��� ������ �������, ��� ��������"
 ����� ����� ���������� "�������"  � "�����", ����� ������������ � 
  "he was more a teacher, than a watchman", � �� �
  "he was bigger a teacher, than a watchman"

   
*/
void CRusSemStructure::ZaplataCompar1(long ClauseNo)
{
  if (m_Clauses[ClauseNo].m_bHasCHEM) 
  if (m_Clauses[ClauseNo].m_ClauseType == COMPARATIVE_T) 
  {

	  for (long CompNodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  CompNodeNo <   m_Clauses[ClauseNo].m_EndNodeNo; CompNodeNo++)
		if (    m_Nodes[CompNodeNo].HasOneGrammem(rComparative)
			&&  (   m_Nodes[CompNodeNo].IsLemma("�������") 		
				 || m_Nodes[CompNodeNo].IsLemma("���������") 		
				)
		   )
		{
			size_t k=CompNodeNo+1;
			for (; k < m_Clauses[ClauseNo].m_EndNodeNo; k++)
			  if (     (		HasRichPOS (k, NOUN) 
							||	HasRichPOS (k, PRONOUN) 
						)
				   &&  m_Nodes[k].HasOneGrammem (rInstrumentalis)
				 )
				 break;
			if ( k == m_Clauses[ClauseNo].m_EndNodeNo ) return;
			if (!m_Nodes[k].IsPrimitive()) return;
			m_Nodes[k].m_Words[0].m_Lemma = "�����";
			InitInterps(m_Nodes[k].m_Words[0], false, ClauseNo, m_Nodes[k]);
		};
	};


};

/*
   ������, ��� � ������� ����� ����� � ���������� ��� "���" � ��� ��� �����
  "������", ��������:
  "�� �� ����� ���� ������ ����"
  "� ���� ��� ������ �������."
�����  ����� ��������� ������� QUANTIT, ������� ���� �� "������"  � ����������  �� ��� ����������������, ��������� "������" ��������� � �������.

*/
void CRusSemStructure::ZaplataCompar2(long ClauseNo)
{
   if (m_Clauses[ClauseNo].m_ClauseSyntaxTop != -1)
   if (    m_Nodes[m_Clauses[ClauseNo].m_ClauseSyntaxTop].HasRelOperator("��") 
	    || m_Nodes[m_Clauses[ClauseNo].m_ClauseSyntaxTop].IsLemma ("���")
	  )
   for (long CompNodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  CompNodeNo <   m_Clauses[ClauseNo].m_EndNodeNo; CompNodeNo++)
		if (    m_Nodes[CompNodeNo].HasOneGrammem (rComparative)
			&&  m_Nodes[CompNodeNo].IsLemma("�����") 		
		   )
		{
			vector<long> Rels;
			GetIncomingSynRelations(CompNodeNo, Rels);
			if (Rels.size() ==  1)
			{
				/*
				 ����� ��������, ����� ��� ����������� ���������� �������� �� �������,
��������, "������ ����" - �����, ����� ���� ����� QUANTIT,  � "����" ������� �������� "��,��",� ��������� ����� QUANTIT �����������, ������� �������� ����� ����������				 ����������
				*/

				assert (m_SynRelations[Rels[0]].m_SynRelName == "���_����_���");
				CRusSemNode& NounNode = m_Nodes[m_SynRelations[Rels[0]].m_SourceNodeNo];
				assert (NounNode.IsWordContainer());
				assert (NounNode.m_MainWordNo != -1);
				NounNode.SetGrammems ( NounNode.m_Words[NounNode.m_MainWordNo].GetAllGrammems());

				DeleteSynRelations(Rels);
			};
			
		};
};

// � �� ���� �������� ������ ����
// � ��� � ��� ���� ������ ����
// ������ ������ ��������
/// === ������������ ������ ���������� ���������� � ������ 
void CRusSemStructure::BuildOutcomingOf_COMP_Obor(long ClauseNo)
{
try {
	int EndNodeNo	= m_Clauses[ClauseNo].m_EndNodeNo;

	/*
		����� ��������� ������	���
		1. "�	������ ��������� ��� ������", 
		2. "��� �����	�� �����, ��� ���� �� ���"
		�����,	����� ���� ��������� ������������������	�������	�� ����	���������� ����
		������ �  �����������
	*/
	for (long	CompNodeNo = EndNodeNo-1;  CompNodeNo >=  m_Clauses[ClauseNo].m_BeginNodeNo; CompNodeNo--)
		if (	m_Nodes[CompNodeNo].m_bCompAdj		)
		{

			if (!m_Nodes[CompNodeNo].IsLemma ("�������"))
				for	(long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  NodeNo <	 EndNodeNo;	NodeNo++)
					if	(		IsVerbForm (m_Nodes[NodeNo])	
							||	HasRichPOS (NodeNo, PREDK) // "�	���� ��� ������	 �������"
						)
					{
						AddRelation(CRusSemRelation(CValency("ACT",A_C), NodeNo, CompNodeNo,  "������_��������������"));			
					};
		}

	for (long CompNodeNo =	EndNodeNo-1;  CompNodeNo >=	 m_Clauses[ClauseNo].m_BeginNodeNo;	CompNodeNo--)
		if	(		(m_Nodes[CompNodeNo].m_MNAType !=	CHEMOborot)
				&&	m_Nodes[CompNodeNo].m_bCompAdj		
			)
		{
			vector<long> ChildNodes;

			size_t	k=CompNodeNo+1;
			for	(;	k <	EndNodeNo; k++)
				if	(		( HasRichPOS (k, NOUN) || HasRichPOS (k, PRONOUN))
						&&  m_Nodes[k].HasOneGrammem(rGenitiv)
					)
				{
					ChildNodes.push_back(k);
					break;
				};

			if (k == EndNodeNo)	
				// � ������, ���	�������	����� -	����������,	 ����� ����	�� ��������	
				//  ���������� ����������� "�	���� �����"
				if (m_Clauses[ClauseNo].m_ClauseType != COMPARATIVE_T) 
					continue;

			/*
			��� ��������� ����� ���� ���������, ��� ��� ��� ��	����� ������ ������� �������
			��������, "�������� ����, ������""
			*/
			if (m_Nodes[CompNodeNo].m_bCompWithOneActant)
				continue;

			// � ��	����  �������� ������ ����
			// ������ ������� ������ ��	�������������� ���������������
			for	(long l=CompNodeNo-1; l	>= m_Clauses[ClauseNo].m_BeginNodeNo; l--)
				if (!m_Nodes[l].m_bSaveIsParenthesis)
					if (HasRichPOS (l, NOUN)	|| HasRichPOS (l, PRONOUN))
						if	(FindFirstRelation(CompNodeNo, l) == -1)
							ChildNodes.push_back(l);


			//	���� ��	�������	�� ����� ��, ��	������ �� �	��.	����� �����������
			//	"������	������ ��������"
			if	(		(m_Clauses[ClauseNo].m_ClauseType	== COMPARATIVE_T) 
					&&	(ChildNodes.size() ==	0) 
				)
				for (size_t k=CompNodeNo+1; k < EndNodeNo;	k++)
					if (	  (	HasRichPOS (k, NOUN) ||	HasRichPOS (k, PRONOUN))
						&&	 m_Nodes[k].HasOneGrammem(rNominativ)
						)
					{
						ChildNodes.push_back(k);
						break;
					};


			if	(	 (ChildNodes.size()	< 2	) )
			{
				if	(	  (m_Clauses[ClauseNo].m_ClauseType	!= COMPARATIVE_T) 
					|| (ChildNodes.size() !=	1) 
					)
					continue;
			};

			//�������� SF1 ����������� �� ����		
			vector<QWORD> SemFets =	GetSemFetsOfFirstValency(CompNodeNo);

			for	(long l=0; l	< ChildNodes.size(); l++)
			{
				AddRelation(CRusSemRelation(CValency(), CompNodeNo, ChildNodes[l],	 ""));
				m_Relations[m_Relations.size()	- 1].m_SemFets = SemFets;
				if	(l == 0) //	������ ����� �����������, �����	�� ��� ������� 
					//	�_���+�, ������	���	����� �� �����
				{
					m_Relations[m_Relations.size() - 1].m_SynReal.m_Cortege.m_DomItemNos[0]	= GetRossHolder(Ross)->IndirObjSynONo;
					m_Relations[m_Relations.size() - 1].m_SynReal.m_Cortege.m_DomItemNos[1]	= GetRossHolder(Ross)->GenitivNo;
					m_Relations[m_Relations.size() - 1].m_Valency.m_RossHolder = GetRossHolder(Ross);
				};
			};


			m_Nodes[CompNodeNo].m_NodeType = MNA;
			m_Nodes[CompNodeNo].m_MNAType =	CHEMOborot;
			break;
		};
}
catch (...)
{
	ErrorMessage ("BuildOutcomingOf_COMP_Obor Failed");
	throw;
};
};


