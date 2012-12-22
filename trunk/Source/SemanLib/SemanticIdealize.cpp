#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"

void CRusSemStructure::DelChildlessCopul ()
{
  // =============================
  // ���� �  Copul ��� �� ������ �������, �� ������� ���� ����
  // �� ������������� ��� ������� �������� 	
  for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
	 if (m_Nodes[NodeNo].m_NodeType == Copul)
	   if ( GetOutcomingRelationsCount(NodeNo,  false) == 0)
	   {
		 DelNode(NodeNo);
		 NodeNo--;
	   };

};

long CRusSemStructure::Idealize ()
{
	long Bonus = 0;
	try {
		// =============================
		// ��� �������� �������� => ������
		// �� ���������  => ���������� ����� (��������������)
		// ALG:  // ����� ����� ������� ��������� � �����, ������� ������������� �� "��" ("c�"). 
		// ������ � ����� ���� ������� ������������ �����.
		// ����� ���������, ����������� �� ������ ����������� ����� ������� (������). ���� ���,
		// �� ����� ������� ����� ���� � ������� (��) � �������� ������� � ������ ������������ 
		// � ����� ����.

		for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
			if (   
				m_Nodes[NodeNo].IsPassiveVerb()
				&& m_Nodes[NodeNo].IsWordContainer() // ����� ���� ���������� ��������������
				&& (m_Nodes[NodeNo].m_Vals.size() > 0)
				)
			{
				vector<long> Rels;
				// ���� ������� � ������� ����������, � �����  � ��������������
				GetOutcomingRelations(NodeNo, Rels, false);
				long i=0;
				for (; i < Rels.size(); i++)
					if (m_Relations[Rels[i]].m_Valency.m_RelationStr == m_Nodes[NodeNo].m_Vals[0].m_RelationStr)
						break;

				if (i < Rels.size())  continue;

				GetOutcomingDopRelations(NodeNo, Rels);
				for (i=0; i < Rels.size(); i++)
					if (m_DopRelations[Rels[i]].m_Valency.m_RelationStr == m_Nodes[NodeNo].m_Vals[0].m_RelationStr)
						break;

				if (i < Rels.size())  continue;


				CRusSemNode N;
				N.m_NodeType = SJA;
				assert (m_Nodes[NodeNo].m_MainWordNo != -1);

				N.m_ClauseNo = m_Nodes[NodeNo].m_ClauseNo;
				long nd = InsertNode(N);
				AddRelation(CRusSemRelation(m_Nodes[NodeNo].m_Vals[0], NodeNo, nd,  ""));

				// ������� "��" �� ����� � �����

				CRusSemWord& W = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo];
				if (HasReflexiveSuffix(W.m_Lemma))
				{
					W.m_Lemma.erase(W.m_Lemma.length() -2, 2);
					W.m_Word.erase(W.m_Word.length() -2, 2);
				};
			}



			// ���� SF  "��" ��� "���" ����� ANIM, �� �����  
			// ��������� �������������� �������������� � �����������, � ��������� ������ ����� ������� 
			// ����������������
			for (long i=0;  i < m_Relations.size(); i++)
				if (m_Relations[i].m_Valency.IsFromDict())
					if (m_Relations[i].m_Valency.m_UnitNo != ErrUnitNo)
					{
						CRusSemNode& N =  m_Nodes[m_Relations[i].m_TargetNodeNo];
						if    (     !N.IsPrimitive()  ) 
							continue;
						if    (     !(N.m_Words[0].m_Lemma == "��")
							&& !(N.m_Words[0].m_Lemma == "���")
							&& !(N.HasOneGrammem(rAnimative) &&  N.HasOneGrammem(rNonAnimative))
				   )
				   continue;
						N.DeleteGrammems (  _QM(rAnimative) | _QM(rNonAnimative) );
						if ( HasSemFetOrLower(N,"ANIM") )				
							N.AddOneGrammem( rAnimative);
						else
							N.AddOneGrammem (rNonAnimative);

					};

			// ====  ��������� ����������
			// ������ �� �������, ������ � ���� ���, �� ������ ����  "��� ������ , ��� ��� �������" � "���1 �� ����" 
			// ��������������� �������������� ��������� ���-���, ���-���� ��� ���_������
			// ��������� �� ��� ��������� ����� ���������, ��������� ����� ���������� �� ��������� ������,
			// ������� ����������� ���������
			for (long i=0;  i < m_Relations.size(); i++)
				if (m_Nodes[m_Relations[i].m_TargetNodeNo].IsPrimitive())
					if (m_Nodes[m_Relations[i].m_TargetNodeNo].m_Words[0].m_Lemma == "���")
					{
						if (HasRichPOS(m_Relations[i].m_SourceNodeNo, ADJ_SHORT))
							m_Relations[i].m_SyntacticRelation = "���_����";
						if (HasRichPOS(m_Relations[i].m_SourceNodeNo,PREDK))
							m_Relations[i].m_SyntacticRelation = "���_������";
						if (HasRichPOS (m_Relations[i].m_SourceNodeNo, ADV))
							m_Relations[i].m_SyntacticRelation = "���_���";
					};


			// MUA[','](x1, ... ,MUA[z](y1, ..., yn), .. xn)  => MUA[z] (x1,...,xn, y1...,yk)  
			// ���� � �������� ������������ A ������ �������� ������������ B, �  �������� A - �������,
			// � �������� B - ����, ����� ��������� � � B ����� �����
			// � ���������� ��� ������� ������������� ���������� ��������������:
			// a+(b+c) = a+b+c
			for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
				if (    (m_Nodes[NodeNo].m_NodeType == MNA)
					&& m_Nodes[NodeNo].IsPrimitive()
					&& (m_Nodes[NodeNo].m_Words[0].m_Word.length() > 0)
					&& (m_Nodes[NodeNo].m_Words[0].m_Word[0] != ',')   
					)
				{
					CRelSet R  = GetIncomingRelations(NodeNo, false);
					long i=0;
					for (; i < R.m_RelsCount; i++)
					{
						const CRusSemNode& N = m_Nodes[m_Relations[R.m_Rels[i]].m_SourceNodeNo];
						if (      N.IsPrimitive()
							&& (N.m_Words[0].m_Word.length() > 0)
							&& (N.m_Words[0].m_Word[0] == ',')   
							)
							break;
					};
					if (i == R.m_RelsCount) continue;
					long ParentNodeNo = m_Relations[R.m_Rels[i]].m_SourceNodeNo;
					EraseRelation(R.m_Rels[i]);
					MoveRelations(ParentNodeNo, NodeNo);
					DelNode(ParentNodeNo);
				};


			// ������ ������� �����������
			InterpretAdjNounGroupsAfter ();
			ApplySubordinationCases ();
			//   ProcessIsolatedParticipleAnd();
			DealInvitatoryMoodAfterTree();
			NumeralAdverbRule();
			FindQuestionClauses();
			FindDividedTermins();
			FindAbbrTermins();
			FindEnglishTermins();
			Bonus += MovePrepNodeToRelationForMainTimeGroups();
			ClearInterpsForFreeTimeGroups();
			Bonus += CreateDefaultSubjectFromPreviousClause();
			ConvertVSE2_toOperator();
			SolveImperativeHomonymy();

	}
	catch (...)
	{
		ErrorMessage("void CRusSemStructure::Idealize failed!");
		throw;
	};

	return Bonus;
};

/*
 ��� ������� ������������  ����, �  ������� ������� ���2, � �������� �������,
 ���� ���� ���� ������� �� �������
 ������: "�� ��� ������ ���������"
 �� �������� ������  ��������� ���������� �������� ApplyKeepNode
*/

void CRusSemStructure::ConvertVSE2_toOperator()
{
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++) 
	{
		if (m_Nodes[NodeNo].GetType() != Ross) continue;
		if (!m_Nodes[NodeNo].IsLemma("���"))  continue;
		if (GetRoss(Ross)->GetUnitMeanNum(m_Nodes[NodeNo].GetUnitNo()) != 2)  continue;
		CRelSet R = GetIncomingRelations(NodeNo, false);
		if (R.m_RelsCount  != 1) continue;
		long nd = m_Relations[R.m_Rels[0]].m_SourceNodeNo;
		if (!IsVerbForm (m_Nodes[nd])) continue;
		m_Nodes[nd].m_RelOperators.push_back("�������");
		if (CanBeDeleted(NodeNo))
		{
			DelNode(NodeNo);
			NodeNo--;
		}
	};
};


void CRusSemStructure::InterpretAdjNounGroupsAfter ()
{
  // ������ ����������� ������� ��� ��������� ����_���, ����_���
  for (long i=0; i < m_Relations.size(); i++) 
   if  (    ( m_Relations[i].m_SyntacticRelation == "����_���" )
	     || ( m_Relations[i].m_SyntacticRelation == "�������_��" )
		 || ( m_Relations[i].m_SyntacticRelation == "���_����_���" )
	   )
   {
	  // swap(m_Relations[i].m_SourceNodeNo, m_Relations[i].m_TargetNodeNo);
	   m_Relations[i].m_Valency.m_Direction = A_C;
   	   if (m_Relations[i].m_Valency.m_RelationStr.empty())
			m_Relations[i].m_Valency.m_RelationStr = "QUANTIT";   
   }
   else
   if  (    ( m_Relations[i].m_SyntacticRelation == "����_���" )
	    ||  ( m_Relations[i].m_SyntacticRelation == "���_����" )
		||  ( m_Relations[i].m_SyntacticRelation == "�����_����" )
	   )
   {
	   //swap(m_Relations[i].m_SourceNodeNo, m_Relations[i].m_TargetNodeNo);
	   m_Relations[i].m_Valency.m_Direction = A_C;
	   if (m_Relations[i].m_Valency.m_RelationStr.empty())
			m_Relations[i].m_Valency.m_RelationStr = "PROPERT";
   }
   else
   if  (    ( m_Relations[i].m_SyntacticRelation == "�����_����" ) )
   {
	   m_Nodes[m_Relations[i].m_SourceNodeNo].m_NodeType  = MNA;
	   m_Nodes[m_Relations[i].m_SourceNodeNo].m_MNAType  = SimpleMNA;

   }
   else
   if  ( m_Relations[i].m_SyntacticRelation == "�����_������" )
   {
	   	   if (m_Relations[i].m_Valency.m_RelationStr.empty())
				m_Relations[i].m_Valency.m_RelationStr = "ASPECT";
   };


   

};


void CRusSemStructure::ApplySubordinationCases ()
{
	// ���� � ��������� �������� ���������� �����, �� ����� ������� �������� � ����-����, ������� 
	// ������������ ����� ������ 
	for (long i=0;  i < m_Relations.size(); i++)
	{
		long MainRelNo = i;
		if (m_Nodes[m_Relations[i].m_SourceNodeNo].IsMNA_ButNotCHEM())
		{
			CRelSet R = GetIncomingRelations(m_Relations[i].m_SourceNodeNo, false);
			if (R.m_RelsCount > 0) MainRelNo = R.m_Rels[0];
		};

		// �� ������������� ������� �� ����� ������������ ������
		if (m_Nodes[m_Relations[MainRelNo].m_TargetNodeNo].HasOneGrammem(rComparative)) continue;

		if (m_Relations[MainRelNo].m_Valency.IsFromDict())
		{
			TCortege& C = m_Relations[MainRelNo].m_SynReal.m_Cortege;
			if (C.m_DomItemNos[1] == -1) continue;
			const CRossHolder* RossHolder  =  m_Relations[MainRelNo].m_Valency.m_RossHolder;
			long CaseItemNo = -1;
			if (   !IsCase(RossHolder, RossHolder->GetSynFet(C) ) ) 
			{

				if (RossHolder->IsLemGroupBegining(RossHolder->GetSynFet(C))) 
				{
					string s =  RossHolder->GetDomItemStrInner(RossHolder->GetSynFet(C)); 
					int j = s.find("+");
					if (j==-1) continue;
					s.erase(0, j+1);
					CaseItemNo = RossHolder->GetItemNoByItemStr (s.c_str(), "D_CASE");
				}
			}
			else
				CaseItemNo = RossHolder->GetSynFet(C);

			if (CaseItemNo == -1) continue;

			QWORD Grammems = GetCaseGrammem(RossHolder, CaseItemNo);

			// ���� � ������� ���� ���������, � ��  �������� �� �_���, ����� 
			// �� ����� ��������� ��� �����������, ��� � ���������
			if (m_Nodes[m_Relations[MainRelNo].m_SourceNodeNo].HasRelOperator("��"))
			 if (RossHolder->GetSynRel( C) ==  RossHolder->DirectObjSynONo)
				Grammems = _QM(rAccusativ) | _QM(rGenitiv);
			 // ��������� �������� �������, ������ ����  ������� � ���������� �������� �����,
			 // � � ������ "����� ����� �������" ��� ������� ��������� �� �����, ���������
			 // ������ � �������� �� ���������.
			if (    (m_Nodes[m_Relations[i].m_TargetNodeNo].m_MainWordNo != -1)
				 && (m_Nodes[m_Relations[i].m_TargetNodeNo].GetGrammems() == m_Nodes[m_Relations[i].m_TargetNodeNo].m_Words[m_Nodes[m_Relations[i].m_TargetNodeNo].m_MainWordNo].GetAllGrammems())
			   )
			  DeleteAllGramCodesWithoutTheGrammems (m_Nodes[m_Relations[i].m_TargetNodeNo], Grammems);
			m_Nodes[m_Relations[i].m_TargetNodeNo].DeleteGrammems( rAllCases);
			m_Nodes[m_Relations[i].m_TargetNodeNo].AddGrammems ( Grammems );
		};
	};

	// ������� ����� - ����� ����� �������� ��. �� ��.
	for (long i=0;  i < m_Relations.size(); i++)
		if	(		(m_Relations[i].m_Valency.m_RelationStr == "QUANTIT")		  
				&&	m_Nodes[m_Relations[i].m_TargetNodeNo].IsPrimitive()
				&&	(m_Nodes[m_Relations[i].m_TargetNodeNo].m_Words[0].m_Word == "1.5")
			)
		{
			m_Nodes[m_Relations[i].m_SourceNodeNo].DeleteGrammems ( rAllNumbers);
			m_Nodes[m_Relations[i].m_SourceNodeNo].AddOneGrammem ( rSingular);
		};
	// �����, ����� ������������
	for (long i=0;  i < m_Relations.size(); i++)
		if	(		(m_Relations[i].m_Valency.m_RelationStr == "QUANTIT") )
		{
			const CRusGramTab *R = (CRusGramTab*)m_piSent->GetOpt()->GetGramTab();
			CRusSemNode& NounN = m_Nodes[m_Relations[i].m_SourceNodeNo];
			CRusSemNode& NumN = m_Nodes[m_Relations[i].m_TargetNodeNo];
			string gcNoun = NounN.m_Words[NounN.m_MainWordNo].m_GramCodes;
			string gc4 = NumN.m_GramCodes;
			if(gc4.length() == 2 && gcNoun.length() == 2) continue;
			gc4 = R->GleicheAncode1(CaseNumberGender0, gc4, 
				R->GetGramCodes(NUMERAL, NounN.GetGrammems() & (rAllCases|rAllGenders), CaseGender),
				gcNoun);
			NumN.ModifyGramCodes(gc4 , 3, R);
			NounN.ModifyGramCodes(gcNoun , 2, R);
		};
	
   // ���� ������� ���������� ���� ������������, � � ���� ���� �������  �� ����������, 
   // ������� ����� ������ ���� � ��., ����  ��., ����� ����� ������� ��������, 
   // ��������������� ����������
   // ��������, "������ ���������"
   //			"����� ���� �������"
   for (long i=0;  i < m_Relations.size(); i++)
	  if ( IsRusSubj (i) )
	  {
		  CRusSemNode& Source = m_Nodes[m_Relations[i].m_SourceNodeNo];
		  CRusSemNode& Target = m_Nodes[m_Relations[i].m_TargetNodeNo];
		  if (Target.m_NodeType  == MNA) continue;
		  bool b_pl = Source.HasOneGrammem(rPlural);
		  bool b_sg = Source.HasOneGrammem (rSingular);
		  if (b_pl && b_sg) continue;

		  // ��������� �� ��������������
		  // ����������� ������� ����� �����������
		  if (!Target.IsILENode())
		  {
			  if (!Target.IsPrimitive()) continue;
			  long ParadigmId = Target.m_Words[0].m_ParadigmId;
			  if (ParadigmId == -1) continue;
			  CFormInfo P;
			  if (! m_pData->GetRusLemmatizer()->CreateParadigmFromID(ParadigmId, P) ) continue;
			  if (P.GetCount() != 1) continue;
			//�������� �� �������������� ���������
		  };

		  Target.DeleteGrammems( rAllNumbers );
		  if (b_pl)
			  Target.AddOneGrammem( rPlural);
		  else
			  Target.AddOneGrammem( rSingular);
	  };

   /*  
     ���� ��������� ����� ����� � ������ (�������� "�_��"),
	 ����� ����� ������������ ��� ����� �� �������� ���� 
     ��������, "������� ���� " �_��
   */
   for (long i=0;  i < m_Relations.size(); i++)
   if (m_Relations[i].m_Valency.IsFromDict())
   {
   		  const CRusSemRelation& R = m_Relations[i]; 
		  const CRossHolder* RossHolder =  R.m_Valency.m_RossHolder;
		  const TCortege& C = R.m_SynReal.m_Cortege;
		  long SynFet = RossHolder->GetSynFet(R.m_SynReal.m_Cortege);
		  if ( !IsCase(RossHolder, SynFet) ) continue;
		  long  CaseGrammems = GetCaseGrammem(RossHolder, SynFet);
		  if ((CaseGrammems & rAllNumbers) == 0) continue;
		  CRusSemNode& Target = m_Nodes[m_Relations[i].m_TargetNodeNo];
		  if ( !Target.HasGrammems (rAllNumbers) ) continue;

		  Target.DeleteGrammems( rAllNumbers );

		  if ((CaseGrammems & _QM(rPlural)) != 0) 
			  Target.AddOneGrammem( rPlural);
          else
			  Target.AddOneGrammem( rSingular );
	};


};


//  ��� ��������� SF(������) = 1. ORG 2. MEDINF, � � ��������������
// "����� ������" -  ������ �������� ������ SF(������) = 1 MEDINF
// ������  ���������, ��� ��� "������" �������� �� MEDINF.
// ��������, � �������������� "����� ������" ������ �������� 
// SF(������) = 1 ORG, ��������� "�����" �������� �� ORG.
void CRusSemStructure::ApplySubordinationSemfets ()
{

	// ���� � ��������� �������� ���������� ����� SF, �� ����� �������� ���  
	// � SF ����, � ������� ������ ��� ��������� � ��������� ����������� ��������
	// � SF ����
	for (long RelNo=0;  RelNo< m_Relations.size(); RelNo++)
	{
		long MainRelNo = RelNo;
		if (m_Nodes[m_Relations[RelNo].m_SourceNodeNo].m_NodeType == MNA)
		{
			CRelSet R = GetIncomingRelations(m_Relations[RelNo].m_SourceNodeNo, false);
			if (R.m_RelsCount > 0) MainRelNo = R.m_Rels[0];
		};

		if (    m_Relations[MainRelNo].m_Valency.IsFromDict()
			&& (m_Relations[MainRelNo].m_Valency.m_UnitNo != ErrUnitNo)
		   )
		{
			const vector<QWORD>& SemFetsRel =  m_Relations[MainRelNo].m_SemFets;
			vector<QWORD> SemFetsNode = m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_NodeSemFets;
			if (!SemFetsRel.empty())
				if (	   SemFetsNode.empty()
						|| m_Nodes[m_Relations[RelNo].m_TargetNodeNo].IsAnaphoricPronoun()
					)
				{
					m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_NodeSemFets = SemFetsRel;
					continue;
				};

            // ���� �� ���� ���������� SF ����
			for (long i=0; i <SemFetsNode.size(); i++)
			{
				// ���� �� ���� ���������� SF ���������
				long k=0;
				for (; k < SemFetsRel.size(); k++)
					if  (SemFetsRel[k] & SemFetsNode[i]) break;
				
                if (k < SemFetsRel.size())
					SemFetsNode[i] = SemFetsRel[k] & SemFetsNode[i];
				else
				{
					SemFetsNode.erase(SemFetsNode.begin() +i);
					i--;
				};

			};

		   if (SemFetsNode.size() > 0)
				m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_NodeSemFets = SemFetsNode;
			

		};
	};

	MoveDopRelationsBack(*this);


};


/*
���� ���� ������ ��� ������� �����, ����������� �����������, ��������:
 ���������� �����������
 ��� ����������
 ������� �������
 ������� ���������
 ��������� ��������
 ����� ������
 ��� �������
 ���� ������
 ����� ����
 ������ ��������
 ����� ������
 �� ����������� ������������
 ���������� ���������
 ������� ������ 
 ������ ������������ 

 ���� � ����� �������  ���� ���������� ��������� �������, ������� �������� �� 
 ������ ������������,����� �������, ��� ������� �������� ������� ��������� 
 � ��������� ����������. � ��������� ������, �������, ������ �����, ���������, � 
 ������������, ������� �������� ��������� �������, ��������:
������ ��� ���� -> SUB(�,����)� ��� ���� ���������� -> SUB(��,����������)�� ������ ���� ������ -> SUB(��,������)
*/

bool HasSynFet (const CSemPattern& P, long ItemNo)
{
	
	for (long i=0;i < P.m_GramCorteges.size(); i++)
		if (P.GetSynFet(i) == ItemNo) 
			return true;

     return false;
	
};

void CRusSemStructure::ApplyDopInfinitive ()
{
    for (long RelNo=0; RelNo < m_Relations.size(); RelNo++)
        if (   HasRichPOS (m_Relations[RelNo].m_TargetNodeNo, INFINITIVE)
            && (         IsVerbForm (m_Nodes[m_Relations[RelNo].m_SourceNodeNo])
            || m_Nodes[m_Relations[RelNo].m_SourceNodeNo].HasPOS(ADJ_SHORT)
            )
            )
        {
            long NodeNo = m_Relations[RelNo].m_SourceNodeNo;
            vector<long> Rels;
            GetOutcomingRelations(NodeNo, Rels, false);
            long DativSubjNodeNo = -1;
            long ParentSubjNodeNo = -1;
            bool HasDativNotFirst = false;
            for (BYTE  ValencyNo = 0; ValencyNo < m_Nodes[NodeNo].m_Vals.size(); ValencyNo++)
            { 
                CSemPattern P;
                GetActantPattern (NodeNo, ValencyNo, P);
                const CValency& V= m_Nodes[NodeNo].m_Vals[ValencyNo];

                long i=0;
                for (;i <Rels.size(); i++)
                    if (m_Relations[Rels[i]].m_Valency == V)
                        break;

                if (i == Rels.size()) i = -1;


                if (ValencyNo == 0)
                    if (i != -1)
                        ParentSubjNodeNo = m_Relations[Rels[i]].m_TargetNodeNo;


                if (ValencyNo > 0)
                    if (!HasDativNotFirst)
                    {
                        HasDativNotFirst |= HasSynFet(P,V.m_RossHolder->DativNo);
                        if (HasDativNotFirst)
                            if ( i != -1)
                                DativSubjNodeNo = m_Relations[Rels[i]].m_TargetNodeNo;
                    };

            };
            long 	InfSubNodeNo = -1;
            if (HasDativNotFirst)
                InfSubNodeNo = DativSubjNodeNo;
            else
                InfSubNodeNo = ParentSubjNodeNo;
            if (InfSubNodeNo == -1) continue;


            NodeNo = m_Relations[RelNo].m_TargetNodeNo;
            if (m_Nodes[NodeNo].m_Vals.size() == 0) continue;
            vector<CValency> ValencyMisses;
            GetValencyMisses (NodeNo, ValencyMisses);
            // � ��������� ������ �� ���������� ������ �����������, �� ���� ����� 
            // ���������, �� ��� �� ��� ������
            if (!_find (ValencyMisses, m_Nodes[NodeNo].m_Vals[0]) ) continue;
            m_DopRelations.push_back(CRusSemRelation(m_Nodes[NodeNo].m_Vals[0],  NodeNo,  InfSubNodeNo,  ""));     
            CSemPattern P;
            GetActantPattern (NodeNo, 0, P);
            m_DopRelations[m_DopRelations.size() - 1].m_SemFets = GetIndexedSemFets(P, true, false);
        };


};

long CRusSemStructure::GetValencyNoWithouProhibited (long NodeNo, long ValencyNo) const
{
  vector<long> Rels;
  GetOutcomingRelations(NodeNo, Rels);

  long Curr =	0; // ������� ����� ���������� �����������
  for (long i=0;i < m_Nodes[NodeNo].m_Vals.size(); i++)
  {
	  long j=0;
	   for (; j <  Rels.size(); j++)
        if (m_Relations[Rels[j]].m_Valency.m_RelationId != m_Nodes[NodeNo].m_Vals[i].m_RelationId)
		if  (!m_Nodes[NodeNo].RelsCanBeTogether 
							(m_Relations[Rels[j]].m_Valency.m_RelationId, 
							 m_Nodes[NodeNo].m_Vals[i].m_RelationId))
							 break;
	 if ( j == Rels.size()) 
		 Curr++;
	 if (Curr == ValencyNo+1)
		 return i;
  };

  return -1;
};

void CRusSemStructure::ApplyDopLexFuncts ()
{
	for (long RelNo=0; RelNo < m_LexFuncts.size(); RelNo++)
		if (IsOper(m_LexFuncts[RelNo].m_LexFunctName) )
		{
			long SubjNodeNo = GetRusSubj(m_LexFuncts[RelNo].m_TargetNodeNo);
			long NodeNo = m_LexFuncts[RelNo].m_SourceNodeNo;
			if (SubjNodeNo == -1) continue;
			CSemPattern P;
			long ValencyNo = -1;
			if (IsOper1(m_LexFuncts[RelNo].m_LexFunctName) )
			  ValencyNo = GetValencyNoWithouProhibited(NodeNo,0);
			else
			   if (IsOper2(m_LexFuncts[RelNo].m_LexFunctName) )
				 ValencyNo =  GetValencyNoWithouProhibited(NodeNo,1);
			  else 
  			   if (IsOper3(m_LexFuncts[RelNo].m_LexFunctName) )
				 ValencyNo =  GetValencyNoWithouProhibited(NodeNo,2);
			   else 
				  continue;
            if (ValencyNo == -1) continue;
			GetActantPattern (NodeNo, ValencyNo, P);
			CRusSemRelation Rel (P.m_PatternValency, NodeNo,  SubjNodeNo,"");
			Rel.m_SemFets = GetIndexedSemFets(P, true, false);
			m_DopRelations.push_back(Rel);     
		};
}


void CRusSemStructure::FindQuestionClauses ()
{
   /*
    ���� ������ ������ ����������� "��� �� �� ������, � ����"
   */
   for (long i=0; i< m_Clauses.size(); i++)
	if (m_Clauses[i].m_HasParticleBY)
	 if (m_Nodes[m_Clauses[i].m_BeginNodeNo].m_bQuestionWord)
	 {
		 vector<long> Nodes;
		 GetClauseRootsWithoutDeleted(i,Nodes);
		 if (Nodes.size() != 1)  continue;
		 if (!m_Nodes[Nodes[0]].HasRelOperator("��")) continue;
		 m_Clauses[i].m_bQuestion = true;
		 m_Nodes[Nodes[0]].m_RelOperators.push_back("no_matter"); 
		 m_Clauses[i].m_HasParticleBY = false;
	 };

	/*
	������ ��������������� ������ ����������� � �������������� ������ � �����
	 */
   if (m_SentenceMood != Interrogative) return;

   vector<long> HostClauses;
   for (long i=0; i< m_Clauses.size(); i++)
   	if (m_Clauses[i].m_HostClause  == -1)
	  HostClauses.push_back(i);

   
   for (long i=0; i< HostClauses.size(); i++)
    if (HostClauses.size() > 1)
	   for (long j=m_Clauses[HostClauses[i]].m_BeginNodeNo; (j < m_Clauses[HostClauses[i]].m_EndNodeNo) && ((j - m_Clauses[HostClauses[i]].m_BeginNodeNo) < ConjWordDistance); j++)
			if (m_Nodes[j].IsLemmaList("�����",  "����",  "����", "���������", "����", "�������", ""))
			{
				HostClauses.erase(HostClauses.begin() +i);
			   i--;
			   break;
			};
       
   if (HostClauses.size() == 1)
   {
	   vector<long> SimilClauses;
	   SimilClauses.push_back(HostClauses[0]);

	   for (long i=0; i< m_Clauses.size(); i++)
		 if (    (m_Clauses[i].m_ClauseRuleNo != -1)
			  && (m_ClauseRules[m_Clauses[i].m_ClauseRuleNo].m_Name == "���������")
			)
		   for (long j=0; j< SimilClauses.size(); j++)
			   if (m_Clauses[i].m_HostClause  == SimilClauses[j])
				   SimilClauses.push_back(i);

	   for (long j=0; j< SimilClauses.size(); j++)
   	     m_Clauses[SimilClauses[j]].m_bQuestion = true;

   };


};


/*
 ������� ������ �������� ����� ������������� � 
 ������������� �����������
 ����������� ������� �����������.
*/
void CRusSemStructure::SolveImperativeHomonymy()
{
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++) 
	{
		if ( !m_Nodes[NodeNo].HasOneGrammem (rImperative) ) continue;
		if ( ! (m_Nodes[NodeNo].GetGrammems()&rAllTimes) ) continue;
		long SubjNodeNo = GetRusSubj(NodeNo);
		if (SubjNodeNo != -1)
		{
		 if (CanBeSubjectForImperative(SubjNodeNo, NodeNo))
			 continue;

		 m_Nodes[NodeNo].DeleteGrammems ( _QM(rImperative) );
		};
	};
};


void CRusSemStructure::dfs_in_clause(size_t NodeNo, vector<long>& Nodes) const
{
  Nodes.push_back(NodeNo);
  for (size_t i=0; i<m_Relations.size(); i++)
	if (m_Relations[i].m_SourceNodeNo == NodeNo)
     if ( !_find(Nodes, m_Relations[i].m_TargetNodeNo) )
		 if (m_Nodes[NodeNo].m_ClauseNo ==  m_Nodes[m_Relations[i].m_TargetNodeNo].m_ClauseNo)
    	  dfs_in_clause(m_Relations[i].m_TargetNodeNo, Nodes);
};


void CRusSemStructure::ConvertParticipleTreeToClause()
{
	int ClauseCount = m_Clauses.size();
	for (long ClauseNo=0; ClauseNo < ClauseCount; ClauseNo++)
		for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo; NodeNo < m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
			if ( m_Nodes[NodeNo].HasPOS(PARTICIPLE) )
			{
				  /*
				    ��������� ��� ��������� ���� ����� ����������
				  */
				  vector<long> Rels;
				  GetOutcomingRelations(NodeNo, Rels, false);
				  long i =0;
				  for (; i< Rels.size(); i++)
					  if (!m_Nodes[m_Relations[Rels[i]].m_TargetNodeNo].IsAbstract())
						break;

				  if (i == Rels.size()) continue;


				  vector<long> Roots;
				  GetClauseRootsWithoutDeleted(ClauseNo,Roots);
				  if (Roots.size() != 1) continue;
				  // ���� ��������� ��� ����� �� ����� ������, ����� ���� �����
				  if (Roots[0] == NodeNo) continue;
				  vector<long> iNodes;
				  dfs_in_clause (NodeNo, iNodes);
				  vector<CRusSemNode> Nodes;
			      SetNodeToDeleteFalse();
				  int NewClauseNo = m_Clauses.size();
				  int NodesCount = m_Nodes.size();
				  for (i =0; i < iNodes.size(); i++) 
				  {
					  Nodes.push_back(m_Nodes[iNodes[i]]);
					  m_Nodes[iNodes[i]].m_bToDelete = true;
					  Nodes.back().m_ClauseNo  = NewClauseNo;
					  MoveAllRelations(iNodes[i], -iNodes[i] - 1);
				  };
				  DelNodesToDelete();
			
			  	  m_Clauses.push_back (CRusSemClause());

				  for (i =0; i < Nodes.size(); i++) 
				  {
					long NewNodeNo = InsertNode(Nodes[i]);
					
					if (i ==  0)
						m_Clauses.back().m_ClauseSyntaxTop = NewNodeNo;

					 MoveAllRelations(-iNodes[i] - 1, NewNodeNo);
					 
				  };
				  m_Clauses.back().m_HostClause = ClauseNo;
				  for (i=0; i < m_ClauseRules.size(); i++)
					  if (m_ClauseRules[i].m_Name == "���������� ������")
					  {
						  m_Clauses.back().m_ClauseRuleNo = i;
						  break;
					  };


			  	 AssertValidGraph();

			};

};

