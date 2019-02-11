
#include "stdafx.h"
#include "SemanticRusStructure.h"

long CRusSemStructure::GetAnaphoricRelationsCount(long Tag)
{
  size_t Result =0;

  for (size_t i = 0;  i < m_DopRelations.size(); i++)
   if (HasTag(m_DopRelations[i].m_SourceNodeNo, Tag))
    if (m_DopRelations[i].m_bRelUse)
 	 if (m_DopRelations[i].m_SyntacticRelation == "�������")
	 		Result++;

  return Result; 
};



bool CRusSemStructure::IsInLexVariant(long ClauseNo, long NodeNo)
{
  return    m_Nodes[NodeNo].IsWordContainer()
         && IsInClause(NodeNo, ClauseNo)
		 ;
};


void CRusSemStructure::InitLexVariant(size_t ClauseNo)
{
  for (size_t NodeNo = 0;  NodeNo < m_Nodes.size(); NodeNo++)
    if (IsInLexVariant(ClauseNo, NodeNo))
		   m_Nodes[NodeNo].ResetCurrInterp();

};

// ���������, ������� �������� ��� ��������, ������ ���� ���-��� ��� ������ ������� �,  
// �� �� ���� ����������� ��������� ���������� ���� �� �, ���� ��� ��������
static void GetLexVariants(  const vector<CDictUnitInterpVector>&  Parents, 
						   CDictUnitInterpVector&   V,
						   vector<CDictUnitInterpVector>&  Variants, 
						   long       Position)
{ 
	if (Variants.size() > 20000) return;
	if (Position < V.size())
	{

		size_t i=0;
		for (; i< Parents[Position].size(); i++)
			{
				size_t k=0;
				for(; k < Position; k++)
					if (V[k] ==  Parents[Position][i])
						break;
				if (k < Position) break;
			};
		if (i == Parents[Position].size())
			for (size_t i=0; i< Parents[Position].size(); i++)
			{
				V[Position] = Parents[Position][i];
				GetLexVariants(Parents, V, Variants, Position+1);
			}
		else
		{
			V[Position] = Parents[Position][i];
			GetLexVariants(Parents, V, Variants, Position+1);
		}
	}
	else
		Variants.push_back(V);

};


bool CRusSemStructure ::  SetLexVariant(size_t ClauseNo, size_t& LexVariantInCurrSetCollocNo, size_t& CurrSetCollocHypNo)
{
StartPoint :

	CRusSemClause& C = m_Clauses[ClauseNo];
	vector<CRusSemNode> SaveNodes = m_Nodes;
	vector<CSynRelation> SaveSynRelations = m_SynRelations;
	SaveClausesNodeReferences();




	// ������ ������� ���������  ��������������
	BuildBlackBoxCollocs(ClauseNo, CurrSetCollocHypNo);

	vector<CDictUnitInterpVector> Parents;


	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
		if (IsInLexVariant(ClauseNo, NodeNo))
			if (!m_Nodes[NodeNo].GetInterps().empty()) 
			{
				Parents.push_back(CDictUnitInterpVector()); 
				m_Nodes[NodeNo].ResetCurrInterp();
				for (size_t k=0; k < m_Nodes[NodeNo].GetInterps().size(); k++)
					Parents[Parents.size() -1].push_back(m_Nodes[NodeNo].GetInterps()[k]);

			}

	// �������� ��������� ���� ��������� �������������
	CDictUnitInterpVector V_curr; // ������� ������� 
	V_curr.resize(Parents.size());
	vector<CDictUnitInterpVector> Variants; // ��� ��������� ��������
	GetLexVariants(Parents, V_curr, Variants, 0);

	// ���� ������� ������� ��������� ��� �������� C.m_SetCollocHyps[m_CurrSetCollocHypNo],
	// ������� � ���������� ��������� ��������������
	if (LexVariantInCurrSetCollocNo >= Variants.size()) 
		if (CurrSetCollocHypNo+1 == m_ClauseSetCollocHyps[ClauseNo].size())
		{
			PopClausesNodeReferences();
			return false;
		}
		else
		{
			CurrSetCollocHypNo++;
			LexVariantInCurrSetCollocNo = 0;
			m_Nodes = SaveNodes;  
			m_SynRelations = SaveSynRelations;
			RestoreClausesNodeReferences();
			goto StartPoint;
		}

	PopClausesNodeReferences();


	// ������������ ������� ��������� �������������
	size_t k = 0;
	CDictUnitInterpVector& V = Variants[LexVariantInCurrSetCollocNo];

	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
		if (IsInLexVariant(ClauseNo, NodeNo))
			if (!m_Nodes[NodeNo].GetInterps().empty()) 
			{
				bool b = m_Nodes[NodeNo].SetCurrInterp(V[k]);
				assert (b);
				k++;		 
			}

	return true;
}

void CRusSemStructure::DeleteSAMNode(long ClauseNo, CRusSemNode& SamNode)
{
	SamNode.m_Words.clear();
	for (long i=0; i < m_Nodes.size(); i++)
		if ( IsInClause(i, ClauseNo) )
			if (m_Nodes[i].IsPrimitive() && CanBeDeleted(i))
				if  (    HasGramFetAfterColon (i, "���") 
					&& HasRichPOS (i, PRONOUN_P)
					)

					if (GetSynRelationsCount(i) == 0)
					{
						SamNode =  m_Nodes[i];
						DelNode(i);
						break;
					}
};

/*
  ��� ������� ���������, ���  ��� ���� "���" � "����" ������� ���������� �������������.
  ����  ������������� ����� GF = *:���, ����� ��� ������  ����������� ������ ��� ������ 
  "�� ������ ��� ���.", ����� "���" �� ��������� ������� ������.
  ���� ������������� �������� GF = *:c_���, ����� ��� ����� ���� ������������ ������ 
  ��� ������ "��� �������� ������ ���".  ����  ������������� ��� "���" � "����" �� ���������������  
  �������� ���. �������������, ����� ������� ������������ �������� ������������� �����.
*/
bool CRusSemStructure::CheckSAMNode(long ClauseNo) const
{
	for (long i=0; i < m_Nodes.size(); i++)
		if ( IsInClause(i, ClauseNo) )
			if (m_Nodes[i].IsPrimitive())
			{
				if  (    HasGramFetAfterColon (i, "���") 
					&& HasRichPOS (i, PRONOUN_P)
					)
				{
					if ( HasIncomingNotWeakSynRelation (i) )  return false;
				};

				if  ( HasGramFetAfterColon (i, "�_���") )
					if  ( m_Nodes[i].IsPrimitive() )
						if (    m_Nodes[i].IsLemma("����")
							||  m_Nodes[i].IsLemma("���")
							)  
							if ( !HasIncomingNotWeakSynRelation (i) )
								return false;
			};

	return true;
};


/*
������� ��������� ���� "���" ��� "����" � �������� ��������� ASPECT � ���� ��� ���� ������������ ����������. ���� �� ������ ��������� �� ������� ��������, 
 ����� ���������� ����.
 ����� ���������� � ����� ��� ����������� ������������.
*/
bool CRusSemStructure::InsertSAMNode(long ClauseNo, CRusSemNode& SamNode)
{
  /*
    ���� ������ ��������� �� ����, ������ � ������� 
  */
  if (SamNode.m_Words.size() == 0) return true;

  long SamNodeNo = InsertNode (SamNode);
  bool bResult = false;

  for (long i=0; i < m_Nodes.size(); i++)
   if ( IsInClause(i, ClauseNo) )
   {
	   CRelSet R = GetIncomingRelations(i, false);
	   for (long l=0; l < R.m_RelsCount; l++)
		if ( m_Relations[R.m_Rels[l]].m_SyntacticRelation == "����" )
			// ��������� ������������ �� �����/����
		  if (    (   (   (_QM(rPlural) | _QM(rSingular))
			            & SamNode.GetGrammems()
			            & m_Nodes[m_Relations[R.m_Rels[l]].m_SourceNodeNo].GetGrammems()
					  )> 0
				  )
               && ( ( (   rAllGenders 
			          & m_Nodes[m_Relations[R.m_Rels[l]].m_SourceNodeNo].GetGrammems()
					  & SamNode.GetGrammems() ) > 0
			         )
			      || (
			          (  rAllGenders 
			          & m_Nodes[m_Relations[R.m_Rels[l]].m_SourceNodeNo].GetGrammems())  == 0
			        )
				  )
			 )
		  {	
			AddRelation(CRusSemRelation(CValency("ASPECT", A_C), m_Relations[R.m_Rels[l]].m_SourceNodeNo,    SamNodeNo,   ""));			
			// ��� ������ ���� �����������, ����� �� ���������� �� ������� 
			// �������, ����� ������ ���������� ����� � ����. �����������.
			m_Relations[m_Relations.size()-1].m_bReverseRel = true;
			m_DopRelations.push_back(CRusSemRelation(CValency("THESAME", A_C),  SamNodeNo,  m_Relations[R.m_Rels[l]].m_TargetNodeNo,  "�������"));			
			bResult = true;
		  }
         else    
		  {
			 // ������� ���������� �������, ������� ��  ������������� �� ������ "���"
			 EraseRelation(R.m_Rels[l]); 
			 // �������� ����� ����� ��������� ���������
			 R = GetIncomingRelations(i, false);
			 l=-1;
		  };
	};
		  
 return bResult;	
};

long Count =0;
void CRusSemStructure::ConnectClausesForLexVariantCombination()
{

	m_Relations.clear();
	m_LexFuncts.clear();
	m_ThesSemRelations.clear();
	m_DopRelations.clear();
	m_SynRelations.clear();

	m_ClausePropertiesProtocol = "";
	m_WeightProtocol = "";

	AssertValidGraph();

	for (long ClauseNo =0; ClauseNo < m_Clauses.size(); ClauseNo++)
	{


		CSemClauseVariantResult I = m_AlreadyBuiltClauseVariants[m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo];

		long CurrLexVariant = m_Clauses[ClauseNo].m_CurrLexVariantNo;
		CLexVariant& W = I.m_BestLexVariants[CurrLexVariant];

		// �������� ����
		/*
		������ ������, � ������� ���������� ������������ ��������
		������ �� �������� ������������� ���� (������ Copul).
		�� ��� �� ���������. ���� �� ��� ���������, ����� �� ����� ��������, ���
		��� ���� ����� ���� WordNo, ������ ��� ������ �� ������
		�������� ����, � ���� � ���������� ���� ��� �� ��������  ����� ����,
		����� � ����������� ������ ����� ������� �� ���� ����, � ��������
		WordNo ��������� � WordNo ������-�� ���� ������� ������.
		*/
		/*
		ClauseNo ����� ���������� �� I.m_BuiltClauseNo, ��������� ����������� 
		������ ����� ���� ��������� � ��������, ��� ��������� ����������� �����
		*/
		long SaveClauseNo = I.m_BuiltClauseNo;
		m_Nodes.erase(m_Nodes.begin() +m_Clauses[ClauseNo].m_BeginNodeNo, m_Nodes.begin() +m_Clauses[ClauseNo].m_EndNodeNo);

		m_Nodes.insert(m_Nodes.begin() + m_Clauses[ClauseNo].m_BeginNodeNo, 
			W.m_Nodes.begin() + W.m_Clauses[SaveClauseNo].m_BeginNodeNo, 
			W.m_Nodes.begin() + W.m_Clauses[SaveClauseNo].m_EndNodeNo);


		// �������� ���������
		long SaveStartNode = W.m_Clauses[SaveClauseNo].m_BeginNodeNo;
		long SaveLength =  W.m_Clauses[SaveClauseNo].GetNodesCount();
		long StartNode = m_Clauses[ClauseNo].m_BeginNodeNo;
		long Length = m_Clauses[ClauseNo].GetNodesCount();
		long Delta = StartNode - SaveStartNode; 

		//W.IncrementNodeRef(W.m_Clauses[SaveClauseNo].m_BeginNodeNo, Delta);
		/*
		������� Delta �� ���� ����� (�� ������ � ����� ������ � ����� ������ ����� ���)
		���� �� ������� ������� ��������� ������ ��� ���� ������,
		����� �������� ���������, ������� �� ����������� ������ ������, �� ���
		����� �������� �� ���� ������ ������ 
		*/
		W.IncrementNodeRef(0, Delta);
		W.m_Clauses[SaveClauseNo].IncrementNodesReferences(Delta);
		m_Relations.insert(m_Relations.end(), W.m_Relations.begin(),W.m_Relations.end());
		m_DopRelations.insert(m_DopRelations.end(), W.m_DopRelations.begin(),W.m_DopRelations.end());
		m_LexFuncts.insert(m_LexFuncts.end(), W.m_LexFuncts.begin(),W.m_LexFuncts.end());
		m_Clauses[ClauseNo] = W.m_Clauses[SaveClauseNo]; 
		m_Clauses[ClauseNo].m_CurrLexVariantNo = CurrLexVariant;
		for (long i=0; i < m_Clauses.size();i++)
			if (m_Clauses[i].m_BeginNodeNo >  m_Clauses[ClauseNo].m_BeginNodeNo)
				m_Clauses[i].IncrementNodesReferences(SaveLength-Length);



		/*
		� W.m_SynRelations ����� m_SourceNodeNo � ����� m_TargetNodeNo
		��� ��������� �� this->m_Nodes, � �� �� W.m_Nodes, ���������
		���� ������� ��������� W.IncrementNodeRef, ������� ��������� ���������
		m_Nodes[W.m_SynRelations[j].m_SourceNodeNo] �
		m_Nodes[W.m_SynRelations[j].m_TargetNodeNo]
		*/
		for  (long j =0; j < 	W.m_SynRelations.size(); j++)
			if  (    (W.m_SynRelations[j].m_SourceNodeNo < m_Clauses[ClauseNo].m_EndNodeNo)
				&& (W.m_SynRelations[j].m_TargetNodeNo < m_Clauses[ClauseNo].m_EndNodeNo)
				&& (m_Clauses[ClauseNo].m_BeginNodeNo <= W.m_SynRelations[j].m_SourceNodeNo)
				&& (m_Clauses[ClauseNo].m_BeginNodeNo <= W.m_SynRelations[j].m_TargetNodeNo)
				)
				m_SynRelations.push_back(W.m_SynRelations[j]);	


		//*    

		/*
		��. ���� ��������� � ������� ����� � � ���������� ������� �����
		*/
		for (long i=m_Clauses[ClauseNo].m_BeginNodeNo;i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
			m_Nodes[i].m_ClauseNo = ClauseNo;

		AssertValidGraph();
		//*	 
		string Q = Format 
			("===== Clause %i ==== \n LexVariantCount = %i\n %s \n Weight = %i, %s \n", 
			ClauseNo, 
			I.m_BestLexVariants.size(),
			W.m_BestValue.Panic ? "Panic" : "All checked",
			W.GetBestTreeWeight(), 
			(W.m_BestValue.ProjectnessCoef == 1) ? "not projected" : "projected");
		m_ClausePropertiesProtocol += Q;
		if (m_Clauses[ClauseNo].m_BestPO != "")
			m_ClausePropertiesProtocol += Format("DOMAIN = %s\n",m_Clauses[ClauseNo].m_BestPO.c_str());


		Q = Format ("\n === Clause %i === \n",ClauseNo);
		m_WeightProtocol += Q;
		m_WeightProtocol += W.m_BestValue.GetStrOfNotNull();
		//*
	};



	// ������� ������� ��� ����������� 
	SetNodeToDeleteFalse();
	SetRelsToDeleteFalse();

	ProcessIsolatedParticipleAnd();

	CLexVariant InitialLexVariant (*this);

	// ������������ ������� ����� ���������� ����������� ������
	m_ClauseConnectionType  = ProminentClauseRules;

	// ��� ������� ��������� ������
	// 1 �������
	ApplyClauseRules( 1);

	// 2 �������
	if (GetClauseRelationCount() != m_Clauses.size() - 1)
	{
		CopyLexVar (InitialLexVariant);
		ApplyClauseRules(m_Clauses.size());
	};

	// 3 �������
	if (GetClauseRelationCount() != m_Clauses.size() - 1)
	{
		CopyLexVar (InitialLexVariant);
		m_ClauseConnectionType	    = AllPossibleClauseRules;
		ApplyClauseRules(m_Clauses.size());

	};


	/*
	����� ������� ��������� � ����, ������� �������� � ��������
	� ������� ApplyClauseRules 
	*/
	DelRelsToDelete();
	DelNodesToDelete();
	AssertValidGraph();


}



CLexVariant CRusSemStructure::BuildTheVariant(long ClauseNo)
{
try 
{
	CLexVariant V;

	V.m_BestValue.Coefs = &m_SemCoefs;

	m_Relations.clear();

	// ����, ��������� ����� "���",��������� �� ����� ���������� ������
	CRusSemNode SamNode;
	DeleteSAMNode(ClauseNo, SamNode);

	bool HypotGraphResult = BuildHypotRelationsGraph(ClauseNo);

	AssertValidGraph();

	PrintRelations();


	if  ( m_PanicTreeVariantCount > 0)
	{
		V.AllRelationsCount =  m_Relations.size();
		V.TreeVariantCount = GetTreeByConnectedComponents(ClauseNo, V.m_BestValue);
	}

	if (!HypotGraphResult)
	{
	  V.m_BestValue.Panic = true;
	};

 
	/*
		����  �� ������� �������� ���� "���", ������ �� �������� ��� ���� ������
	*/
	if (!InsertSAMNode(ClauseNo, SamNode))
		V.m_BestValue.ConnectedComponentsCount++;
		  
	V.m_BestValue.LexFunctsCount = CountLexFunctsInOneClause(ClauseNo);
	V.m_BestValue.GramRestrViolationsCount = 0;
	return V;
}
catch (...)
{
	 ErrorMessage ("BuildTheVariant Failed");
	 throw;
};
};



//��������������� ����� �����, �����, ���������, �������,
// ���� ��� ���� �������� ��������, ���� "�� � ���"

void UnitDisruptedConjunctions (CRusSemStructure& R, long ClauseNo)
{
	for (long i = R.m_Clauses[ClauseNo].m_BeginNodeNo; i < R.m_Clauses[ClauseNo].m_EndNodeNo; i++)
		if (R.m_Nodes[i].IsLemmaList("���","���","�������","�����", ""))
		{
			//"��" ����� ������ ��������� ��� ���� ��������� ��������� 
			if  (     (i > 0)
					&& R.m_Nodes[i-1].IsWordForm("��")
					&& R.CanBeDeleted(i-1)
				)
				R.DelNode(i-1);
			else
				if  (     (i > 1)
						&& R.m_Nodes[i-2].IsWordForm("��")
						&& R.CanBeDeleted(i-2)
					)
					  R.DelNode(i-2);
				else
					continue;
			// ������ ��� ������� ����
			i--;


			CRusSemWord& W = R.m_Nodes[i].m_Words[0];
			W.m_Lemma = "��" + W.m_Lemma;
			W.m_ParadigmId = R.m_pData->GetFirstParadigmId(morphRussian, W.m_Lemma,  0);
			R.m_Nodes[i].DelAllInterps();

			string UnitStr = W.m_Lemma;
			EngRusMakeLower(UnitStr);
			WORD UnitNo  = R.GetRossHolder(Ross)->LocateUnit(UnitStr.c_str(), 1);
			if (UnitNo != ErrUnitNo)
				for (long j = UnitNo; UnitStr == R.GetRoss(Ross)->GetEntryStr(j); j++)
					if (R.GramFetAgreeWithPoses (*R.GetRossHolder(Ross), j, W))
						R.m_Nodes[i].AddInterp( CDictUnitInterp(R.GetRossHolder(Ross),Ross, j, false, false) );

		};


};

// �������� �� ������������ �����, ����������� ������� "��", "������" � ������� ����� ����.
void CRusSemStructure::InitRelationOperators (long ClauseNo)
{
	UnitDisruptedConjunctions(*this, ClauseNo);
	//if (m_Clauses[ClauseNo].GetNodesCount() == 0)
	//	return

	SetNodeToDeleteFalse();

	// ���������� ���������
	for (long i=m_Clauses[ClauseNo].m_BeginNodeNo; i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
	{
		// ���������� ���������� ����� ���� �����
		StringVector PrefixRelOperators;
		long k=i;
		for (; (k < m_Clauses[ClauseNo].m_EndNodeNo) && CanBePrefixRelationOperator(k); k++)
			if (		!m_Nodes[k].IsTimeRossNode() 
					&&	(m_Nodes[k].m_MNAType != RepeatMNA) 
					&&	CanBeDeleted(k)
				// ������� ���� "�� ..., ��" �� �������� ����������, ���� ��������� "��" - ��������
				)
			{
				m_Nodes[k].m_bToDelete = true;
				PrefixRelOperators.push_back(m_Nodes[k].m_Words[0].m_Lemma);
			};

		// ��������� ���������� ���������, ����  ��� ���� �������, � ����� ��� ���  ������
		// ���� �� ���� �����, �� ���. ����. ���������� ( �-� ����)
		if (k <  m_Clauses[ClauseNo].m_EndNodeNo)
		{
			for (long l=0; l <  PrefixRelOperators.size(); l++)
				m_Nodes[k].m_RelOperators.push_back(PrefixRelOperators[l]); 
		}
		else
		{

			for (long j=i; j < k; j++)
				m_Nodes[j].m_bToDelete = false;


		}

		i = k;
	};

	// ����������� ���������
	long MainNodeNo = 0; // ������� ����� - � ��������  ����� ����������� ���������
	for (long i=m_Clauses[ClauseNo].m_BeginNodeNo+1; i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
		if ( CanBePostfixRelationOperator(i)  )
		{
			m_Nodes[MainNodeNo].m_RelOperators.push_back(m_Nodes[i].m_Words[0].m_Lemma);
			m_Nodes[i].m_bToDelete = true;
		}
		else
			MainNodeNo = i;

	DelNodesToDelete();
};


static void GetIndexedVariants(const vector<CLexVariantWeightVector>&  Parents, 
									   VectorLong&   V,
							  vector<VectorLong>&  Variants, 
								long       Position)
{ 
	if (Variants.size() > 10000) return;
	if (Position < V.size())
		for (long i=0; i< Parents[Position].size(); i++)
		{
			V[Position] = i;
			GetIndexedVariants(Parents, V, Variants, Position+1);
		}
	else
		Variants.push_back(V);

};

// returns the weight of the best tree variant

long CRusSemStructure::FindSituationsForClauseVariantCombination(  )
{
	const long constMinStructureWeight = 1000000;
	m_bHasConjBut = false;
   //::MessageBox(0, "111", "111", MB_OK);

  	StartTimer("������������� ����������",0);
	try {
      BuildSemNodesBySyntax();
	}
	catch (...)
	{
		ErrorMessage("������ � ������������� ����������");
	    throw;
	};


	if (m_Nodes.size() == 0) return constMinStructureWeight;

	#ifdef _DEBUG
		PrintLemNodes();
		AssertValidGraph();
	#endif
	EndTimer("������������� ����������");

	rml_TRACE ("=================================================\n");
	rml_TRACE ("===  FindSituationsForClauseVariantCombination %i===\n",  m_ClauseVariantsCombinationNo);

	ProcessParticipleAndInTheFirstClause();
	
	m_ClauseSetCollocHyps.clear();
	m_ClauseSetCollocHyps.resize(m_Clauses.size());
//*


	// ������� �������� ����� ��� ����. ��������� �����
    for (long ClauseNo =0; ClauseNo < m_Clauses.size(); ClauseNo++)
    {
		m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo = FindAlreadyBuiltClauseVariant(ClauseNo);
		if (m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo == -1)
		{

			m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo = m_AlreadyBuiltClauseVariants.size();
			CSemClauseVariantResult ClauseVar (m_Clauses[ClauseNo].m_SyntaxClauseNo, m_Clauses[ClauseNo].m_ClauseVariantNo, m_Clauses[ClauseNo].m_AddedClauses);
			ClauseVar.m_BuiltClauseNo = ClauseNo;
			m_AlreadyBuiltClauseVariants.push_back(ClauseVar);
		};
	};

    for (long ClauseNo =0; ClauseNo < m_Clauses.size(); ClauseNo++)
    {

		if (m_bShouldBeStopped) return constMinStructureWeight;
		
     
		



		CSemClauseVariantResult& ClauseVar = m_AlreadyBuiltClauseVariants[m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo];
		if (ClauseVar.m_bNew)
		{

			BuildTimeNodes(ClauseNo);

			InterpretRepeatConj(ClauseNo);

			// InitRelationOperators ������ ������ ��  ������ ��������������, ��
			// ����� ������ ��������� ������, ����� �� ����� ���������� ������ ����
			// "�� ��������, �� �����  ����", ��� "��" ����������� ������������� � "��������"
			InitRelationOperators(ClauseNo);
			
			InterpretPrepNouns(ClauseNo);

			InterpretSelectiveRelations(ClauseNo);


			ReadDopFieldForClause(ClauseNo);

			ReadThesInterps(ClauseNo);

		    FindConceptFetsFromArticles(ClauseNo);


			DealInvitatoryMoodBeforeTree(ClauseNo);

			FindCollocsHyps(ClauseNo);

			InitLexVariant(ClauseNo); 

			CLexVariant InitialLexVariant;
			InitialLexVariant.CopyLexVar(*this);

			// ����� ���������� ������������� ���������������� ��������
			ClauseVar.m_bNew = false;
			m_InterfaceClauseNo ++;

			ZaplataCompar2(ClauseNo);
			

			size_t LexVariantInCurrSetCollocNo = 0;
			size_t CurrSetCollocHypNo = 0;
			

			// ���� �� ���� ����������� ��������� 
			for (;;)
			{
				if (m_bShouldBeStopped) return constMinStructureWeight;
		  
		  
				if (!SetLexVariant(ClauseNo, LexVariantInCurrSetCollocNo, CurrSetCollocHypNo)) 
				{ 
					CopyLexVar(InitialLexVariant);
					break; 
				};

	      

				bool bCheckSAMNode = CheckSAMNode(ClauseNo);

		  
				CLexVariant V = BuildTheVariant(ClauseNo);
				PrintRelations();
				PrintNodes();
				ClauseVar.m_NodesCount  += m_Clauses[ClauseNo].GetNodesCount();
				ClauseVar.m_PanicRelationsCount += V.AllRelationsCount;
				V.m_BestValue.SAMNodeViolation = !bCheckSAMNode;
				V.m_LexVariantNo = LexVariantInCurrSetCollocNo;
				V.m_SetCollocHypNo = CurrSetCollocHypNo; 
				V.m_BestValue.CollocsCount = m_ClauseSetCollocHyps[ClauseNo][CurrSetCollocHypNo].size();
				V.CopyLexVar(*this);

			   
				rml_TRACE ("������ %i\n", ClauseNo);
				rml_TRACE ("����������� �������(ClauseNo = %i) =  %i, ����� ��-��� = %i(�� %i)\n", ClauseNo, LexVariantInCurrSetCollocNo+1,  CurrSetCollocHypNo+1,m_ClauseSetCollocHyps[ClauseNo].size());
				rml_TRACE ("��� ������� �������� ������ %i\n", V.GetBestTreeWeight());
				ClauseVar.m_BestLexVariants.push_back(V);
				rml_TRACE ("��������  ������ %i\n", IsConnectedClause(ClauseNo));
				CopyLexVar(InitialLexVariant);
				LexVariantInCurrSetCollocNo++;
				/*
				���� ����������� ��������� ������ 100, ����� ���� ��������
				�� ���������� ��������
				*/
				if (LexVariantInCurrSetCollocNo > 100)
					break;

				
			} // ����� �����

	  	 // ����������� ������ ���������
			sort (ClauseVar.m_BestLexVariants.begin(),ClauseVar.m_BestLexVariants.end());
			for ( long i=0; i < ClauseVar.m_BestLexVariants.size(); )
				if ( ClauseVar.m_BestLexVariants[i].GetBestTreeWeight() > ClauseVar.m_BestLexVariants[0].GetBestTreeWeight() )
					ClauseVar.m_BestLexVariants.erase(ClauseVar.m_BestLexVariants.begin() +i );
				else
					i++;

		}
	 
	
	
	}
    
//*
	long BestCombNo = -1;
	vector<VectorLong> Variants;
	rml_TRACE ("=================================================\n");
	rml_TRACE  ("===  Connecting clauses   %i ===\n", m_ClauseVariantsCombinationNo);
	m_InterfaceClauseNo ++;

	// ���������� �������� ��������
	VectorLong V;
	
	V.resize(m_Clauses.size());
	vector<CLexVariantWeightVector> Parents;
	for (long i=0; i <m_Clauses.size(); i++)
	{
		long k = m_Clauses[i].m_AlreadyBuiltClauseVariantNo;
		CLexVariantWeightVector V;
		for (long j=0; j  < m_AlreadyBuiltClauseVariants[k].m_BestLexVariants.size(); j++)
			V.push_back(m_AlreadyBuiltClauseVariants[k].m_BestLexVariants[j]);
		Parents.push_back(V);
	};
	GetIndexedVariants(Parents, V,Variants, 0); 

	
	long MinWeight = constMinStructureWeight;
	
	CLexVariant InitialLexVariant;
	InitialLexVariant.CopyLexVar(*this);

	for (long i=0; i  < Variants.size(); i++) 
    {
		if (m_bShouldBeStopped) return constMinStructureWeight;
   
		for (long ClauseNo=0; ClauseNo <m_Clauses.size(); ClauseNo++)
		{
			m_Clauses[ClauseNo].m_CurrLexVariantNo =  Variants[i][ClauseNo];
		};


		ConnectClausesForLexVariantCombination ();

		long Weight = GetStructureWeight();
		if (Weight < MinWeight)
		{
			MinWeight = Weight;
			BestCombNo = i;
		};

		CopyLexVar(InitialLexVariant);
	};


	AssertValidGraph();


	rml_TRACE  ("=================================================\n");
	rml_TRACE  ("===  ������ ������� ��������  %i ===\n", m_ClauseVariantsCombinationNo);

	if (BestCombNo != -1)
	{
		for (long ClauseNo=0; ClauseNo <m_Clauses.size(); ClauseNo++)
			m_Clauses[ClauseNo].m_CurrLexVariantNo =  Variants[BestCombNo][ClauseNo];

		ConnectClausesForLexVariantCombination ();
 
		if (m_ClauseVariantsCombinationNo == 95)
		{
			int uu =0;
		}
		BuildAnaphoricRels(); 

		long Improvements = Idealize();

		/*if ( Improvements > 0 )
		{
			CSemClauseVariantResult& ClauseVar = m_AlreadyBuiltClauseVariants[m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo];
			bool bCheckSAMNode = CheckSAMNode(ClauseNo);
			CLexVariant V = BuildTheVariant(ClauseNo);
			PrintRelations();
			PrintNodes();
			ClauseVar.m_NodesCount  += m_Clauses[ClauseNo].GetNodesCount();
			ClauseVar.m_PanicRelationsCount += V.AllRelationsCount;
			V.m_BestValue.SAMNodeViolation = !bCheckSAMNode;
			V.m_LexVariantNo = LexVariantInCurrSetCollocNo;
			V.m_SetCollocHypNo = CurrSetCollocHypNo; 
			V.m_BestValue.CollocsCount = m_ClauseSetCollocHyps[ClauseNo][CurrSetCollocHypNo].size();
			V.CopyLexVar(*this);


			rml_TRACE ("������ %i\n", ClauseNo);
			rml_TRACE ("����������� �������(ClauseNo = %i) =  %i, ����� ��-��� = %i(�� %i)\n", ClauseNo, LexVariantInCurrSetCollocNo+1,  CurrSetCollocHypNo+1,m_ClauseSetCollocHyps[ClauseNo].size());
			rml_TRACE ("��� ������� �������� ������ %i\n", V.GetBestTreeWeight());
			ClauseVar.m_BestLexVariants.push_back(V);
			CTreeOfLexVariantWeight W = m_AlreadyBuiltClauseVariants[m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo].m_BestLexVariants[m_Clauses[ClauseNo].m_CurrLexVariantNo];
			Weight += W.GetBestTreeWeight1(!IsConn);

		}*/
		return GetStructureWeight() + Improvements;


	}
	else
		return constMinStructureWeight;
	
 
};


long CRusSemStructure::GetStructureWeight()
{
	bool IsConn = IsConnected();

	long Weight = 0;

	for (size_t i =0; i < m_Clauses.size(); i++)
	{
		/*
			����� ����������� ����. ��������, ����� ���������� ��������, ����� 
			m_Clauses[i].m_AlreadyBuiltClauseVariantNo �� ������������������, �����
			����� ������� ���� ���������� (���� ����� ������� �����)
		*/
		if (m_Clauses[i].m_AlreadyBuiltClauseVariantNo == -1)
		{
			 return  100000;
		};

		if ( m_AlreadyBuiltClauseVariants[m_Clauses[i].m_AlreadyBuiltClauseVariantNo].m_BestLexVariants.empty() )
			Weight += 10000;
		else
		{
		   CTreeOfLexVariantWeight W = m_AlreadyBuiltClauseVariants[m_Clauses[i].m_AlreadyBuiltClauseVariantNo].m_BestLexVariants[m_Clauses[i].m_CurrLexVariantNo];
		   Weight += W.GetBestTreeWeight1(!IsConn);
		   //rml_TRACE("%d %s\n", W.GetBestTreeWeight1(!IsConn), W.m_BestValue.GetStr().c_str());
		 };
	};

	long Tag = SetTagToSentence();
	long SemanticVolume = GetSemanticVolume(Tag);  
	long RelsCount = GetUseRelationsCount();  
	TreeVariantValue Summa;
	Summa.Coefs = &m_SemCoefs;
	// ������������� �� ������, ������� ��������� � ���������� ( 5 ������ )
	Summa.SemFetDisagree = RelsCount ? GetSemFetDisagreeCount(Tag)*1000 / RelsCount : 0;
	Summa.LexFetAgreeCount = RelsCount ? GetLexFetAgreeCount(Tag)*1000 / RelsCount : 0;
	Summa.OptionalValencyCount = GetOptionalValenciesCount(Tag);
	Summa.InstrAgentRelsCount = SemanticVolume ? GetInstrAgentRelsCount(Tag)*1000  / SemanticVolume : 0;
	Summa.CorporaGleicheCount = GetCorporaGleicheCount(Tag);
    if (Summa.CorporaGleicheCount == 1)
	  Summa.CorporaGleicheCount = 0;

	// ������ �� ����� ��������� ������ �� ����� ������ �� ����������� ������ 
	// ������, �� ���� ������, �� ��������� ���������� � �������, � �� � ������. 
	// ���� ������ �� ����� ���������.
	Summa.RelationsLength = 0;


	// ��� ����� �������, �� � ����� �����?
    Summa.ObligatoryValencyViolationCount = GetObligatoryValencyViolation(Tag);

	// ����� �������, ��������� ��� ����������� ������� ����������, ��������, ��������� �����.
	Summa.LexFunctsCount = m_LexFuncts.size();
    UpdateBlockedRelations();
	if (!IsConn)
       if (RelsCount == 0)
		   Summa.RelationsLength = 0;
	   else
		   Summa.RelationsLength = SemanticVolume ? GetRelationsLength(Tag)*1000/RelsCount : 0;

	Weight += Summa.GetTreeWeight();



	Weight -= GetAnaphoricRelationsCount(Tag) * 5;
	//rml_TRACE("%s\n", Summa.GetStr().c_str());
	/*
	 ���� ���� ������������ ������������� �����������  �����, �����
	 ����� ���� ����� ���� ���������
	*/
	if (AllPossibleClauseRules == m_ClauseConnectionType)
		Weight += 60;

	return Weight;
};

struct CClauseComplexity
{
	long      m_ClauseNo;
	float     m_Complexity;
	CClauseComplexity (long      ClauseNo, float     Complexity)
	{
	  m_ClauseNo = ClauseNo;
	  m_Complexity = Complexity;
	};
	string GetStr()
	{
	   return Format ("������=%i, ���������=%.2f",
		   m_ClauseNo, m_Complexity );
	};
};

string CRusSemStructure::GetClauseComplexitiesStr()
{
   vector<CClauseComplexity> ComplexClauses;
   for (long j=0;j <m_AlreadyBuiltClauseVariants.size(); j++)
   {
	   
	   long i=0;
	   for (; i< ComplexClauses.size(); i++)
		   if (ComplexClauses[i].m_ClauseNo == m_AlreadyBuiltClauseVariants[j].m_SyntaxClauseNo)
			   break;

       if (i == ComplexClauses.size())
	   {
		   double Value = 0;
		   long  Count = 0;
		   for (long i=0; i<m_AlreadyBuiltClauseVariants.size(); i++)
		     if (m_AlreadyBuiltClauseVariants[i].m_SyntaxClauseNo == m_AlreadyBuiltClauseVariants[j].m_SyntaxClauseNo)
			 {
				 if (m_AlreadyBuiltClauseVariants[i].m_NodesCount > 0)
					Value += (double)m_AlreadyBuiltClauseVariants[i].m_PanicRelationsCount/(double)m_AlreadyBuiltClauseVariants[i].m_NodesCount;
				 Count++;
			 };

          if (Count > 0)
			  ComplexClauses.push_back(CClauseComplexity(m_AlreadyBuiltClauseVariants[j].m_SyntaxClauseNo, Value/Count));
	   };

   };
   string Result;
   for (long j=0;j < ComplexClauses.size();j++)
	   Result += ComplexClauses[j].GetStr() + string("\n");
   return Result;

};




bool  CRusSemStructure::ReadAuxiliaryArticles()
{
   if ( GetRoss(Ross) == NULL) return false;

   WORD UnitNo = GetRossHolder(Ross)->LocateUnit("_����",1);
   if (UnitNo == ErrUnitNo) return false;

   if (!GetRoss(Ross)->IsEmptyArticle(UnitNo))
	  for (size_t i = GetRoss(Ross)->GetUnitStartPos(UnitNo); i<= GetRoss(Ross)->GetUnitEndPos(UnitNo); i++)
	  {
		TCortege C = GetCortege(GetRoss(Ross), i);
		string S = WriteToString(GetRoss(Ross), (char*)(GetRoss(Ross)->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
		Trim(S);
		if (!m_SemCoefs.ReadOneCoef (S.c_str()))
		  {
			  ErrorMessage (string(S) + string(" is not recognized as a semantic coefficient"));
		  };
	  };

   UnitNo = GetRossHolder(Ross)->LocateUnit("_weak_syn_rel",1);
   if (UnitNo == ErrUnitNo) return false;
   m_WeakSynRels.clear();

   if (!GetRoss(Ross)->IsEmptyArticle(UnitNo))
	  for (size_t i = GetRoss(Ross)->GetUnitStartPos(UnitNo); i<= GetRoss(Ross)->GetUnitEndPos(UnitNo); i++)
	  {
   		long ItemNo = GetCortege(GetRoss(Ross),i).m_DomItemNos[0];
		if (ItemNo != -1)
		{
          string OneSynRel =  GetRossHolder(Ross)->GetDomItemStrInner(ItemNo);
		  m_WeakSynRels.push_back(OneSynRel);
		};
	  };

   sort(m_WeakSynRels.begin(), m_WeakSynRels.end());

   return true;

};

bool CRusSemStructure::GetClauseVariantCombination()
{
	vector<VectorLong> Parents;
	Parents.resize(m_piSent->GetPrimitiveClausesCount());
	m_AllClausesVariants = 0;
	for (long i=0; i < Parents.size(); i++)
	{
		for (long k=0; k < m_piSent->GetPrimitiveClause(i)->m_SynVariants.size(); k++)
		{
			Parents[i].push_back(k);
			m_AllClausesVariants++;
		}
	};
	VectorLong V; // ������� ������� 
	V.resize(Parents.size());
	vector<VectorLong> Variants;
	GetCommonVariants(Parents, V, Variants, 0);



	//GetClauseVariantCombinations(Variants);

	if (m_ClauseVariantsCombinationNo  >= Variants.size()) return false;
	m_Clauses.clear();
	m_Clauses.resize(m_piSent->GetPrimitiveClausesCount());
	for (long i=0; i < m_Clauses.size(); i++)
	{
		const CClause* pClause = m_piSent->GetPrimitiveClause(i);
		m_Clauses[i].m_ClauseVariantNo = Variants[m_ClauseVariantsCombinationNo][i];
		CSVI pSynVar = pClause->GetSynVariantByNo(m_Clauses[i].m_ClauseVariantNo);

		int RootNo = pSynVar->m_ClauseTypeNo;
		if (RootNo != -1)
		{
			m_Clauses[i].m_ClauseType = pClause->m_vectorTypes[RootNo].m_Type;
			m_Clauses[i].m_RootWordNo = pClause->m_vectorTypes[RootNo].m_Root.m_WordNo;
		}
		else
		{
			m_Clauses[i].m_ClauseType = UnknownSyntaxElement;
			m_Clauses[i].m_RootWordNo = -1;
		}
		m_Clauses[i].m_SyntaxClauseNo = i;

		m_Clauses[i].m_SubjectWordNo = -1;
		if (!pSynVar->m_Subjects.empty())
			m_Clauses[i].m_SubjectWordNo = pSynVar->m_Subjects[0];
		m_Clauses[i].m_PredicateWordNo = pSynVar->m_iPredk;

	};

	rml_TRACE ("����� ������ �������������� ��������� ������ %i (�� %i)\n", m_ClauseVariantsCombinationNo+1, Variants.size());
	m_ClauseCombinationVariantsCount = Variants.size();
	m_Nodes.clear();
	return true;
};



long CRusSemStructure::FindSituations(size_t SentNo)
{
	//m_bTimeSpanHolderEnabled = false;

	try {
		m_AllClausesVariants = 0;
		ClearTimers();
		StartTimer("����� ����� ������",0);

		if (!ReadAuxiliaryArticles())
		{
			ErrorMessage ("Cannot read ross auxiliary articles");
			return  -1;
		};

		Queries.clear();
		m_MemoryCaseNo = 0;
		m_MemRelations.clear();
		m_MemNodes.clear();

		StartTimer("������������� ����������",0);

		m_AlreadyBuiltClauseVariants.clear();

		m_IndexedSemFets.clear();
		m_IndexedSemFets.push_back("NEG");
		m_IndexedSemFets.push_back("CAUS");
		m_IndexedSemFets.push_back("FIN");
		m_IndexedSemFets.push_back("SOC");

		m_bLastTry = false;
		m_piSent =	m_pData->GetSynan()->m_vectorSents[SentNo];
		m_ClauseCombinationVariantsCount = 0;
		m_bShouldBeStopped = false; 





		m_pData->InitializeIndices();
		m_InterfaceClauseNo = 0;


		EndTimer("������������� ����������");
		long BestClauseVariantsCombinationNo = 0;
		long BestClauseVariantsCombinationWeight = 100000;

		bool  bTooSlow = false;    
		double WordsPerSecond = 0;
		size_t ProcessedVariantsCount = 0;
		if (m_UserClauseVariantsCombinationNo != -1)
		{
			BestClauseVariantsCombinationNo = m_UserClauseVariantsCombinationNo;
			m_ClauseVariantsCombinationNo = -1;
		}
		else
			for (m_ClauseVariantsCombinationNo = 0; GetClauseVariantCombination() ; m_ClauseVariantsCombinationNo++)
			{
				ProcessedVariantsCount++;
				m_ClausePropertiesProtocol = "";
				if (m_bShouldBeStopped) return -1;
				StartTimer("FindSituationsForClauseVariantCombination",0);
				long Weight = FindSituationsForClauseVariantCombination();

				if (m_bShouldBeStopped) return -1;


				rml_TRACE ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");
				rml_TRACE ("����� ��� �������� %i = %i \n", m_ClauseVariantsCombinationNo, Weight);
				rml_TRACE ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");

				if (Weight  < BestClauseVariantsCombinationWeight)
				{
					BestClauseVariantsCombinationNo = m_ClauseVariantsCombinationNo;
					BestClauseVariantsCombinationWeight = Weight;
				};

				double AllTicks = EndTimer("FindSituationsForClauseVariantCombination");
				WordsPerSecond =  (AllTicks == 0) ?  0 : (double)(m_Nodes.size())  / (AllTicks/CLOCKS_PER_SEC);

#ifndef _DEBUG

				/*
				� ��������  ������, ����  �������� ������, ��� ���������� �� �����,
				����� ���� ��������.
				*/
				if ( (WordsPerSecond < 0.5) &&  (WordsPerSecond > 0))
				{
					bTooSlow = true;
					break;
				};
				
				if (ProcessedVariantsCount >  100)
				{
					bTooSlow = true;
					break;
				};

#endif

			};


		if (m_ClauseVariantsCombinationNo - 1 ==  BestClauseVariantsCombinationNo)
			m_ClauseVariantsCombinationNo = BestClauseVariantsCombinationNo;
		else
		{
			m_ClauseVariantsCombinationNo = BestClauseVariantsCombinationNo;
			rml_TRACE ("--------------------------------------------------------------\n");
			GetClauseVariantCombination();

			m_bLastTry = true;
			BestClauseVariantsCombinationWeight = FindSituationsForClauseVariantCombination();

			AssertValidGraph();

		};

		ConvertParticipleTreeToClause();

		string S = Format("����� ���� � ����������� %i\n ����� �������� � �������: %i\n ������ ������� ����� : %i (�� %i)\n",
			m_piSent->m_Words.size(), Queries.size(), BestClauseVariantsCombinationNo+1, m_ClauseCombinationVariantsCount);

		EndTimer("����� ����� ������");



		m_TimeStatictics =    string(GetStrRepresentation().c_str()) + S;

		m_TimeStatictics += GetClauseComplexitiesStr()+string("\n");

		S = Format("==========\n ��� �����������: %i\n", BestClauseVariantsCombinationWeight);
		m_ClausePropertiesProtocol +=  S;
		S = Format("����� ����. ����. ���. : %i\n", m_UserProhibitedLexVars.size());
		m_ClausePropertiesProtocol +=  S;
		S = Format("�������� (�����/�������) =  %10.0f\n", WordsPerSecond);
		if (bTooSlow)
		{
			S += "����� �� �������� !!!!\n";
		};

		m_ClausePropertiesProtocol +=  S;





		m_ClauseVariantsStatistics = "";
		for (long j=0;j <m_AlreadyBuiltClauseVariants.size(); j++)
			m_ClauseVariantsStatistics += m_AlreadyBuiltClauseVariants[j].GetStr() + string ("\n");

		return  BestClauseVariantsCombinationWeight;
	}
	catch  (...)
	{
		ErrorMessage ("long CRusSemStructure::FindSituations(size_t) Failed");
		throw;
	};
};



bool CRusSemStructure::GetSyntaxTreeByText(string text, int ClauseVarNo, string& Graph)
{
	try 
	{
		CMyTimeSpanHolder			GlobalSpan; 
		m_pData->MakeSyntaxStr(text.c_str(), GlobalSpan);
		if (m_pData->GetSynan()->m_vectorSents.empty())
			return false;
		m_piSent =	m_pData->GetSynan()->m_vectorSents[0];
		if (ClauseVarNo != -1)
		   m_ClauseVariantsCombinationNo = ClauseVarNo;
		else
		   m_ClauseVariantsCombinationNo = 0;
		GetClauseVariantCombination();
		BuildSemNodesBySyntax();
		m_TimeStatictics = Format("����� ���� � ����������� %i\n ���-�� ����. ��������� : %i\n",m_piSent->m_Words.size(),  m_ClauseCombinationVariantsCount);

		Graph =  GetTclSyntaxGraph ();

		return true;
	}
	catch (...)
	{
		return false;
	};
}

bool CRusSemStructure::SetLemmasToReplace(string LemmasToReplace)
{
	m_SynthLemmaToReplace.clear();
	
	RmlMakeUpper(LemmasToReplace,morphRussian);
	StringTokenizer tok(LemmasToReplace.c_str(), ";");
	while (tok())
	{
		char lemma1[255];
		char lemma2[255];
		string OnePair = tok.val();
		if (sscanf(OnePair.c_str(), "%[^/]/%[^/]", lemma1, lemma2) != 2)
			return false;
		string lem1=lemma1;
		Trim(lem1);
		if (lem1.empty()) return false;
		string lem2=lemma2;
		Trim(lem2);
		if (lem2.empty()) return false;
		m_SynthLemmaToReplace[lem1] = lem2;
	};
	return true;
};



