#include "StdAfx.h"
#include "EngSemStructure.h"
#include "EngSynthes.h"



bool CEngSemStructure::init_helper()
{
	helper.text_kind = translate_helper::TEXT_HUD;
	helper.SetData( RusStr.m_pData );
	helper.init_private_lists();
	return true;
}



void CEngSemStructure::CreateEngClauses()
{
	m_Clauses.clear() ;
	long EngClauseCount = 0;
	for(int i = 0 ; i < m_Nodes.size() ; i++ )
	{
		assert(m_Nodes[i].m_ClauseNo != -1);
		if (m_Nodes[i].m_ClauseNo+1 > EngClauseCount)
			EngClauseCount = m_Nodes[i].m_ClauseNo+1;
	}
	m_Clauses.resize(EngClauseCount);
	for (int i =0; i < m_Clauses.size(); i++)
	{
		m_Clauses[i].Copy(RusStr.GetSemClause(i));
	};

}





void CEngSemStructure::IndexSemFets()
{
	for (long NodeNo=0; NodeNo <m_Nodes.size(); NodeNo++)
	{
		CSemPattern P;
		m_Nodes[NodeNo].m_NodeSemFets.clear();
		// ��������� SF �� ��������� ������ 
		if (m_Nodes[NodeNo].GetUnitNo() != ErrUnitNo)
		{
			P.InitSemPattern(GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].GetUnitNo(),0, 0);
			if (P.LoadSemFromDict())
				m_Nodes[NodeNo].m_NodeSemFets = GetIndexedSemFets(P, false, true);
		};

		// ���� � ��������� ������ ��� SF, �����
		// ��������� SF �� ���������
		if (m_Nodes[NodeNo].m_NodeSemFets.size() == 0)
			if (m_Nodes[NodeNo].RusNode != -1)
				InitThesSemFet(m_Nodes[NodeNo], RusStr.GetNode(m_Nodes[NodeNo].RusNode));

		// ���� � ���������  ��� SF, �����
		// ��������� SF �� �������� ����
		if	(		m_Nodes[NodeNo].m_NodeSemFets.empty()
				&&	(m_Nodes[NodeNo].RusNode != -1)
			)
		 {
			 const CSemNode& N =  RusStr.GetNode(m_Nodes[NodeNo].RusNode);
			 for (long i=0; i < N.m_NodeSemFets.size(); i++)
				 P.m_ActantSemFets.push_back(GetSemFetStr (N.m_NodeSemFets[i]));
			 m_Nodes[NodeNo].m_NodeSemFets = GetIndexedSemFets(P, false, true);
		 };

		// ���������� ������ ����� ����� ������������

	};

};



void CEngSemStructure::TranslateActants(int iEngNode, int iClause)
{
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);	

	for( int i=0; i<outRels.size(); i++ )
	{
		CSemRelation& semRel = m_Relations[outRels[i]];	
		if( m_Nodes[semRel.m_SourceNodeNo].m_ClauseNo != iClause )
			return;
	
		TranslateOneActant(outRels[i],iEngNode);
		
		if( m_Nodes[semRel.m_TargetNodeNo].m_bReached )
			continue;				
		m_Nodes[semRel.m_TargetNodeNo].m_bReached = true;
		TranslateActants(semRel.m_TargetNodeNo,iClause);
	}	
}



string CEngSemStructure::BuildSentence()
{	
	string sent;
	try
	{
		CEngSynthes EngSynthes(*this);

		sent = EngSynthes.BuildSentence();
	}
	catch(exception &e)
	{
		return string(e.what());
	}
	catch(...)
	{
		return "Unknown error in Synthes";
	}

	return sent;
}



bool CEngSemStructure::Init()
{
	try
	{
		m_Nodes.clear();
		m_Relations.clear();
		m_Nodes.clear();
		m_Relations.clear();
		m_LexFuncts.clear();

		if( m_SemRelToGramCortegeMap.size() == 0 )
		{
			FillSemRelToGramCortegeMap();
			FillSemRelEquivs();
		}

		init_helper();
	}
	catch(...)
	{
		return false;
	}
	return true;
}

bool CEngSemStructure::WasInterpreted (long RusNodeNo) const
{
	return find(m_InterpretedRusNodes.begin(),m_InterpretedRusNodes.end(),RusNodeNo) !=m_InterpretedRusNodes.end();
}

void CEngSemStructure::SetInterpreted (long RusNodeNo) 
{
	m_InterpretedRusNodes.push_back(RusNodeNo);
}

long CEngSemStructure::GetClauseFirstWordNo(long ClauseNo) const
{
	long MinWordNo = 10000;
	for( long i=0; i<m_Nodes.size(); i++ )
		if( m_Nodes[i].m_ClauseNo == ClauseNo )
			if(m_Nodes[i].IsWordContainer() )
				if( m_Nodes[i].GetMinWordNo() < MinWordNo )
					MinWordNo = m_Nodes[i].GetMinWordNo();
	return MinWordNo;
}

long CEngSemStructure::GetClauseLastWordNo(long ClauseNo) const 
{
	long MaxWordNo = 0;
	for ( long i=0; i<m_Nodes.size(); i++ )
		if (m_Nodes[i].m_ClauseNo == ClauseNo)
			if (m_Nodes[i].IsWordContainer())
				if (MaxWordNo < m_Nodes[i].GetMaxWordNo())
					MaxWordNo = m_Nodes[i].GetMaxWordNo();
	return MaxWordNo;
}




string CEngSemStructure::GetMorphologyOfNode(long NodeNo) const 
{
  if  (m_Nodes[NodeNo].m_MainWordNo == -1) 
	  return GetNodeLemStr (NodeNo);

  const CEngSemWord& W = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo];


  string POS = "  ";
  if (W.m_Poses != 0)
  {
    long pos =  GetOnePOS(W.m_Poses);
	if (pos != -1)
      POS = string(m_pData->GetEngGramTab()->GetPartOfSpeechStr(pos));
  };

  string Result =   Format ("%s = %s %s", 
	  GetNodeLemStr (NodeNo).c_str(),  
	  POS.c_str(), 
	  m_pData->GetEngGramTab()->GrammemsToStr(W.GetAllGrammems()).c_str()
	 );

  if (W.GetTense() != zero_tn)
  {
	  Result +=  string ("\nTense = ")+ string(W.m_bMorphologicalPassiveForm?"PassiveForm ": "Active ") +
		  GetTenseString(W.GetTense()) +string("\n");
  };

  if (W.m_TenseHistory.size() > 1)
  {
    Result += "\nTense History\n";
	for (int i=0; i< W.m_TenseHistory.size(); i++)
	{
		Result += GetTenseString(W.m_TenseHistory[i].m_Tense);
		Result += "\t";
		Result += W.m_TenseHistory[i].m_Maker; 
		Result += "\n";
	};
  };

  if (W.m_bDoNotChangeForm)
  {
	Result += " DoNotChangeForm \n"; 
  };
  
  if (m_Nodes[NodeNo].m_ArticleStr != "")
  {
	Result += "\nArticle  = " + m_Nodes[NodeNo].m_ArticleStr + "\n"; 
	Result += "ArticleHistory  = " + GetArticleCauseHistory(m_Nodes[NodeNo].m_ArticleCauseHistory);
	
  };

  Result += "\nNotUseTo  = " + string(m_Nodes[NodeNo].m_bNotUseTo?"yes":"no")+ "\n"; 

	
  return Result;
};


void CEngSemStructure::AssertValidGraph()
{
  CSemanticStructure::AssertValidGraph();

 // �������� ������������ ���� WordNo
 for( int i=0; i<m_Nodes.size(); i++)
 {
		int iMainWordNo = m_Nodes[i].m_MainWordNo;
		if( iMainWordNo==-1 )
			continue;
		int iWordNo = m_Nodes[i].m_Words[iMainWordNo].m_WordNo;
		assert ( iWordNo !=-1 );
 }

  // �������� ���� ClauseNo
 for(int i=0; i<m_Nodes.size(); i++)
 {
	 assert ((m_Nodes[i].m_ClauseNo  != -1) && (m_Nodes[i].m_ClauseNo  < m_Clauses.size()) );
 };

};

void  CEngSemStructure::InitSyntaxRelations ()
{
	for (long i=0; i < m_Relations.size(); i++)
		if (IsSubjPattern(m_Relations[i]) )
			m_Relations[i].m_SyntacticRelation = "subj";
		else
		if ( IsObjPattern(m_Relations[i]) 
			&& m_Relations[i].m_SyntacticRelation != "indir_obj" )
			m_Relations[i].m_SyntacticRelation = "obj";
};

void  CEngSemStructure::SetPositionOfChildrenByGrammems (long NodeNo, QWORD Grammems, string Position)
{
	vector<long> outRels;
	GetOutcomingRelations(NodeNo,outRels);	
	for( int i=0; i<outRels.size(); i++ )
	{
		CSemRelation& semRel = m_Relations[outRels[i]];	
		if (m_Nodes[semRel.m_TargetNodeNo].GetGrammemsRich() & Grammems)
		{
			semRel.m_Position = Position;
			semRel.m_PosType = FromAlgorithmPosType;
		};
	};
};

/*
  ������: ������ "the...the"
  ���� ����������� ������� � ������, ���� � ������, ���� ��� ���� ���� ���� 
  POSi = g : p, ��� i=1,2, g - ���. ��������, p - �������. ����� i-�� ������
  �����, ���� ��� ��� �������, � ������� ���� �������� g, ����������� �� ������� p.
*/
void  CEngSemStructure::SetPositionsFromConj ()
{
	for (long i=0; i < m_Relations.size(); i++)
	{
		const CSynRealization &syn = m_Relations[i].m_SynReal;
		const CRossInterp &conj = syn.m_Conj;
		if(conj.m_DictType == EngObor)
		{
			for (long LeafId =0; LeafId < 2; LeafId++)
			{
				vector<TCortege> Corteges;
				GetRossHolder(EngObor)->GetFieldValues ("POS", conj.m_UnitNo, Corteges, 1, 0);
				for (long j=0; j < Corteges.size(); j++)
				{
					TCortege C = Corteges[j];
					if  (  (C.m_DomItemNos[0] == -1) 
						 ||(C.m_DomItemNos[1] == -1) 
						)
						continue;
						
					string GrammemsStr = GetRossHolder(EngObor)->GetDomItemStrInner(C.m_DomItemNos[0]);
					GrammemsStr = "NOUN " + GrammemsStr;
					BYTE POS;
					QWORD Grammems;
					bool b = m_pData->GetEngGramTab()->ProcessPOSAndGrammemsIfCan(GrammemsStr.c_str(), &POS, &Grammems);
					if (!b) continue;
					string Position = GetRossHolder(EngObor)->GetDomItemStrInner(C.m_DomItemNos[1]);
					long NodeNo = (LeafId ==0) ? m_Relations[i].m_SourceNodeNo : m_Relations[i].m_TargetNodeNo;
					SetPositionOfChildrenByGrammems (NodeNo, Grammems, Position);
				};
			}
		};
	};

}

/*
���� ��������� �����_�� ���� �����(one, boy) � ��������� ��� � ���������� ����������� 
one of the boy. ��� ����� ���� ������������ ������� �������� ������� ������.
*/
void  CEngSemStructure::SetSelectiveRelations()
{
	for (long RelNo=0; RelNo< m_Relations.size(); RelNo++)
	if ( m_Relations[RelNo].m_SyntacticRelation == "�����_��" )
	{
		MoveIncomingRelations(m_Relations[RelNo].m_SourceNodeNo,m_Relations[RelNo].m_TargetNodeNo);
		MoveIncomingDopRelations(m_Relations[RelNo].m_SourceNodeNo,m_Relations[RelNo].m_TargetNodeNo);
		m_Nodes[m_Relations[RelNo].m_SourceNodeNo].m_SynReal.m_Preps =  m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_SynReal.m_Preps;
		ReverseRelation(RelNo);
		SetSimpleEngPrep("of",-1, RelNo);
		m_Relations[RelNo].m_Position = ">>";
	};
};


/*
���� � ������� �������� ����� ���� ������������ (���, �� � ��� ���-��)
� � ����� ����������� ���� ����������� ���������������, � � ������ ���, �����
����������� ��� ���������������� ������ ���� �� ���������� � ���������������.
��������, I give the box which you like to you - �����
		� ����� 	I give to you the box which you like 
*/
void  CEngSemStructure::SetLongRelations()
{
 for( int NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
 {
	if ( m_Nodes[NodeNo].m_NodeType == MNA ) continue;
	if ( IsAuxVerb(NodeNo) ) continue;

 	vector<long> outRels;
	GetOutcomingRelations(NodeNo,outRels);	
	vector<int> ClauseRelNos;
	vector<int> InnerRelNos;
	for( int i=0; i<outRels.size(); i++ )
	if  (       !m_Relations[outRels[i]].m_bReverseRel
		   &&   !IsRelBetweenClauses(m_Relations[outRels[i]]) 
		)
	if ( IsSubj(m_Relations[outRels[i]]) )
	{
		outRels.erase(outRels.begin() + i);
		i--;
	}
	else
	{
		vector<long> Rels;
		GetRelations(m_Relations[outRels[i]].m_TargetNodeNo, Rels);
		long j=0;
		for (; j < Rels.size(); j++)
			if  ( IsRelBetweenClauses(m_Relations[Rels[j]]) )
				break;
		
			if (j < Rels.size())
				ClauseRelNos.push_back(outRels[i]);
			else
				InnerRelNos.push_back(outRels[i]);
	};

	if (   (ClauseRelNos.size() > 0)
		&& (InnerRelNos.size() > 0)
	   )
	{
		for (int i=0; i < ClauseRelNos.size(); i++)
		{
			m_Relations[ClauseRelNos[i]].m_Position = ">";
			m_Relations[ClauseRelNos[i]].m_PosType = FromAlgorithmPosType;
		};
		for (int i=0; i < InnerRelNos.size(); i++)
		{
			m_Relations[InnerRelNos[i]].m_Position = ">>";
			m_Relations[InnerRelNos[i]].m_PosType = FromAlgorithmPosType;
		};

	}
 };
};





// ����������������������� ��������� ����� ��������� ��������� to
bool CEngSemStructure::TranslateCasesIfNeed (long NodeNo)
{
	vector<long> Rels;
	GetIncomingRelations(NodeNo, Rels, false); 
	if (Rels.size() != 0) return false;
	if (m_Nodes[NodeNo].m_SynReal.m_Preps.size()  > 0) return false;
	if (m_Nodes[NodeNo].RusNode == -1) return false;
	const CSemNode& N = RusStr.GetNode(m_Nodes[NodeNo].RusNode);
	if (   ((N.GetGrammems() & rAllCases) != _QM( rDativ) )
		&& ((N.GetGrammems() & rAllCases) != ( _QM( rDativ) | _QM( rLocativ) ) )
	   )  return false;
	SetSimpleEngPrep("to", NodeNo, -1);
	return true;
};



string CEngSemStructure::GetInterfaceWordStr(const CSemNode* pNode, int WordNo) const 
{
	CEngSemNode& Node = *(CEngSemNode*) pNode;
	string L = Node.m_Words[WordNo].m_Word;
	if (Node.m_Words[WordNo].m_NumeralPrefix != "")
		L = Node.m_Words[WordNo].m_NumeralPrefix + "-" + L;

	if( !Node.m_Words[WordNo].m_PosleLog.m_PosleLog.empty() )
	{
		L += " (";
		L += Node.m_Words[WordNo].m_PosleLog.m_PosleLog;
		L += ")";
	}
	else
	if( Node.m_Words[WordNo].m_PosleLog.m_PosleLogPrep.m_DictType != NoneRoss )				
	{
		L += " (";
		L += GetRoss(Node.m_Words[WordNo].m_PosleLog.m_PosleLogPrep.m_DictType)->GetEntryStr(Node.m_Words[WordNo].m_PosleLog.m_PosleLogPrep.m_UnitNo);
		L += ")";
	};

    return L;
};

void CEngSemStructure::GetColorAndWidthOfRelation(int RelNo,float& Width,string& Color) 
{
	if( m_Nodes[m_Relations[RelNo].m_SourceNodeNo].m_ClauseNo != 
		m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_ClauseNo )
	{
		if( m_Relations[RelNo].m_bInterpreted )
			Color = "blue";
		else 
			Color = "darkcyan";
		Width = 2;
	}
	else 
	{
		if( m_Relations[RelNo].m_bInterpreted )
			Color = "red";
		else 
			Color = "darkgreen";
		if( IsSubj(m_Relations[RelNo]) )
			Width = 2;
		else
			Width = 1;
	}
}




bool CEngSemStructure::TranslateToEnglish( )
{

	try
	{
		Init();

		CEnglishEquivMap mapRNodeToENode;
		m_SentenceMood = RusStr.m_SentenceMood;
		m_IndexedSemFets = RusStr.m_IndexedSemFets;
		m_bHasConjBut = RusStr.m_bHasConjBut;
		m_InterpretedRusNodes.clear();

		
// ������� ��� ���������� �����������
		FindEnglishEquivMain(mapRNodeToENode);
		ResetAllReachedFlags<CEngSemStructure>(*this);

		vector<long> Roots;
		RusStr.GetRoots(Roots);
		
		for( int i=0; i<Roots.size(); i++ )
		{
			SetInterpreted(Roots[i]);
			InterpretRusNodeAndItsChildren(mapRNodeToENode,Roots[i]);			
		}
		MoveDopRelationsBack(*this);
		FilLexFunctRel();
		PrintNodes();
		CreateEngClauses();
		PrintRelations();
		

		
		 //������ ������ ��� ��������, ������� � ������� ���������  ����
		 //���� ������. ����� ������� ���������������� ���� ������� ����������� ���������� ��������
		 //�� ���������, ���� ��������-����������  �� ����.
		 //� ����� ������, � ��� ������ �� ����� -1 �SemNode::m_TerminId.
		 //��.. ������������ �������������� �������� (translate_abbr_termin_node)
		
		for (int i=0; i< m_Nodes.size(); i++)
		if (    ( m_Nodes[i].m_TerminId != -1)
			 && !m_Nodes[i].m_ThesAbbrForm
		   )
		{
			const CThesaurus* Thes = helper.GetThes(m_Nodes[i].m_ThesaurusId);
			int TerminNo = Thes->GetTerminNoByTextEntryId(m_Nodes[i].m_TerminId);
			set_multiword_termin (i, &Thes->m_Termins[TerminNo], m_Nodes[i].m_ThesaurusId);
		}

	}
	catch (...)
	{
		ErrorMessage("An error occured on the interpretation");
		return false;

	};
//*	
	try {

		// �������������� �������� SF 
		IndexSemFets();

// ��������� �������
		ResetAllReachedFlags<CEngSemStructure>(*this);
		m_InterpretedRusNodes.clear();
		
		for( int ClauseNo=0; ClauseNo<m_Clauses.size(); ClauseNo++ )
		{
			vector<long> Roots;
			Roots.clear();
			GetClauseRoots(ClauseNo, Roots);

			for( long i=0; i<Roots.size(); i++ )
			{
				m_Nodes[Roots[i]].m_bReached = true;
				TranslateActants(Roots[i], ClauseNo);
			}
		}
	}
	catch (...)
	{
		ErrorMessage("An error occured while translating valencies");
		return false;

	};



	try 
	{
// ��������� ����� ���������
		vector<long> algNodes;
	
		for( int i=0; i<m_Nodes.size(); i++ )
			RefineByNumeralMarks(i);

		for( int i=0; i<m_Nodes.size(); i++ )
			RefineSynRealMarks(i);

		for( int i=0; i<m_Nodes.size(); i++ )
			AddLexFuncNode(i);

		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ApplyALG_AL1(algNodes[j]);

		for( int j=0; j<m_Nodes.size(); j++ )
			ApplyALG_AL3(j);

		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ApplyNoRule(algNodes[j]);
		
		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ApplyBeRule(algNodes[j]);
		
		for( int i=0; i<m_Nodes.size(); i++ )
			ApplyKeepRule(i);

		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ApplyALG_it(algNodes[j]);

		//���������� �������������������� �������,
		AddFixedGrammemsToNode();
		InitArticleField();

		for( int i=0; i<m_Nodes.size(); i++ )
			ApplyComparativeRule(i);


		for( int i=0; i<m_Nodes.size(); i++ )
			ApplyAdjShortRule(i);

		PrintNodes();

		for( int i=0; i<m_Nodes.size(); i++ )
			TranslateAdverbAdjectiveRule(i);


		PrintNodes();
		PrintRelations();


		for( int i=0; i<m_Nodes.size(); i++ )
			ApplyPredicativeRule(i);

		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ApplyVerbToAdjRule(algNodes[j]);

		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ImpersonalVerb(algNodes[j]);

		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			InfinitiveAndIf(algNodes[j]);

		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ApplyInvitatoryRule(algNodes[j]);

		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ApplyModalCopulRule(algNodes[j]);

		for( int i=0; i<m_Nodes.size(); i++ )
			ApplyTwoNegationsRule(i);

////

// ��������� ����������� ���������
		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
			ApplyALG_compl_obj(algNodes[j]);

		TranslateClauseRels();

		// ����� TranslateClauseRels ���������� ����� �������� InitArticleField ��-�� ����, ���
		// � TranslateClauseRels ��������� ���������� � which-������, �������  ������ �� �������
		InitArticleField();

		RefineEngCollocPreps();
		RefineComparativeMNAwithPreps();
		RefineUnknownRelsSynRel();
		CorrectNodeNumByRelNum();

// "RUSETENSE"
		GetAlgNodes(algNodes);
		for( int j=0; j<algNodes.size(); j++ )
		{
			CEngSemNode& engNode = m_Nodes[algNodes[j]];
			if( !engNode.IsWordContainer() )
				continue;
			if( engNode.RusNode==-1 )
				continue;
			if( RusStr.GetNode(engNode.RusNode).IsTimeRossNode() )
				continue;
			EngVerbTenseEnum AVREM_Tense = handle_AVREM_field(algNodes[j],true,engNode.RusNode);
			if( AVREM_Tense != zero_tn )
			{
				engNode.m_Words[engNode.m_MainWordNo].SetTense(AVREM_Tense,"�� ���� RUSETENSE");
				CorrectTenseByDictVerbFeatures(engNode); // "RESTR" "not_cont"
			}
		}

// �������������� �����������
		if( !HandleQuestionNode() )
		{
			for( long ClauseNo=0; ClauseNo<m_Clauses.size(); ClauseNo++ )
			{
				int iEngRoot = GetMainClauseRoot(ClauseNo);
				int iRusNode = m_Nodes[iEngRoot].RusNode;
				if( iRusNode==-1 )
					continue;
				int iRusClau = RusStr.GetNode(iRusNode).m_ClauseNo;
				bool bIsQ = RusStr.IsQuestionClause(iRusClau);
				if( !bIsQ )
					continue;
				HandleQuestionClause(iEngRoot);
			}
		}
	}
	catch (...)
	{
		ErrorMessage("An error occured while applying algorithms");
		return false;

	};

	try{
		// ==============	�������� ����-������
		ResetAllReachedFlags<CEngSemStructure>(*this);
		for( long i=0; i<m_Nodes.size(); i++ )
			// ��������� ������� ����, ������� ����� ������ �������
			if (translate_time_node(i)) i = -1;
	}
	catch (...)
	{
		ErrorMessage("An error occured while translating time-groups");
		return false;

	};		
	
	//*
		// ==============	��������� �������
	try {
		for ( long i=0; i<m_Nodes.size(); i++ )
			// ��������� ������� ����, ������� ����� ������ �������
			if (translate_termin_node(i)) i = -1;

		for ( int i=0; i<m_Nodes.size(); i++ )
			translate_abbr_termin_node(i);
		
	}
	catch (...)
	{
		ErrorMessage("An error occured while translating termins");
		return false;

	};		
		// ==============	��������� �������� �������� ����������� ����
	try{
		for( long i=0; i<m_Nodes.size(); i++ )
			translate_binary(i);
	}
	catch (...)
	{
		ErrorMessage("An error occured while browsing binary dictionary");
		return false;

	};		
	//*
	try {
		
		 //���������� �������������������� �������, ��� ���� ����� (���� 
		 //����� ������ ��� ���, ������� ������� �������)
		
		AddFixedGrammemsToNode();

		ConvertClosedCollocToOpen();
		CreateOneselfNodes();


  	   for(long  i=0; i<m_Nodes.size(); i++ )
			TranslateCasesIfNeed(i);

		InitSyntaxRelations();


		SetPositionsFromConj();
		
		HideCopul();
		ReverseClauseRel();
//
		ApplySequenceOfTenseRule();
		ApplyModalVerbTenseRule(); // "can" | "must" -> "be able"+(to) | "have"+(to)
		BuildAuxiliaryVerbs();

//Gri - ������ ��� ������� �� ������, � ������� � �� ������		
		if( !IsConnected() )
		{
			for( int i=0; i<m_Relations.size(); i++ )
			{
				if( m_Relations[i].m_PosType != FromAlgorithmPosType )
					m_Relations[i].m_Position = "";
			}
		}

		SetSelectiveRelations();
		PrintRelations();

		SetLongRelations();
	}
	catch(...)
	{
		ErrorMessage("An error occured while applying post-algorithms");
		return false;
	}

	
	PrintNodes();
	AssertValidGraph();
	return true;
};

