/////////////////////////////////////////////////////////////////////////////
// ���������� ��������� 2

// ApplyModalCopulRule()

// ApplyNoRule()
// ApplyBeRule()
// ApplyKeepRule()

// ApplyALG_it()
// ApplyALG_compl_obj()

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// ApplyModalCopulRule() "������" �.�.� 
// ex. "��� ������ ����" -> "I have no place to go"
//  "������ ����" -> "There is no place to go"
// --> ������� have | there be

void CEngSemStructure::ApplyModalCopulRule(int iEngNode)
{
	CEngSemNode engNode = m_Nodes[iEngNode];
	if( engNode.m_NodeType != ModalCopul )
		return;

	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);

	int iSubjRel = -1, iVerbRel = -1;
	for( int i=0; i<outRels.size(); i++ )
	{
		CSemRelation& engRel = m_Relations[outRels[i]];
		if( engRel.m_Valency.m_RelationStr == "SUB" )		
		{
			iSubjRel = outRels[i];
			continue;
		}
		if	(			engRel.m_Valency.m_RelationStr == "CONTEN"
				||		engRel.m_Valency.m_RelationStr == "ModalCopul" 
			)
		{
			iVerbRel = outRels[i];
			continue;
		}
	}

// ������ ���� ����� �� ModalCopul �� ������ 
	if( iVerbRel==-1 )
	{
		DelNode(iEngNode); // ��� ����� � ���� ����� ������ ������
		return;
	}
	
	int iVerbNode = m_Relations[iVerbRel].m_TargetNodeNo;
	int iMCopRel = -1;

// ������ ���� ����� �� ������� �� ���������� ModalCopul ������� �����
// ���� ��� MUA (������ ������ � �����)	
	if( m_Nodes[iVerbNode].m_NodeType == MNA )
	{
		vector<long> mnaRels;
		GetOutcomingRelations(iVerbNode,mnaRels);
		int iTestVerb = -1;
		for( int j=0; j<mnaRels.size(); j++ )
		{
			iTestVerb = m_Relations[mnaRels[j]].m_TargetNodeNo;
			outRels.clear();
			GetOutcomingRelations(iTestVerb,outRels);
			for( int i=0; i<outRels.size(); i++ )
			{
				int iTrg = m_Relations[outRels[i]].m_TargetNodeNo;
				if( HasNeg(m_Nodes[iTrg].GetUnitNo(),m_Nodes[iTrg].GetType()) )
				{
					iMCopRel = outRels[i];
					break;
				}
			}
			if( iMCopRel!=-1 )
			{
				iVerbNode = iTestVerb;
				break;
			}
		}
	}
	else
	{
		outRels.clear();
		GetOutcomingRelations(iVerbNode,outRels);
		for( int i=0; i<outRels.size(); i++ )
		{
			int iTrg = m_Relations[outRels[i]].m_TargetNodeNo;
			if( HasNeg(m_Nodes[iTrg].GetUnitNo(),m_Nodes[iTrg].GetType()) )
			{
				iMCopRel = outRels[i];
				break;
			}
		}
	}

// ���� ����� �� �������, ���������� ����� ������ ���� ������ ���������
	if( iMCopRel==-1 )
	{
		outRels.clear();
		GetOutcomingRelations(iEngNode,outRels);
		for( int i=0; i<outRels.size(); i++ )
		{
			int iTrg = m_Relations[outRels[i]].m_TargetNodeNo;
			if( HasNeg(m_Nodes[iTrg].GetUnitNo(),m_Nodes[iTrg].GetType()) )
			{
				iMCopRel = outRels[i];
				break;
			}
		}
	}
	assert( iMCopRel!=-1 );
	if( iMCopRel==-1 )
		return;

	int iRusNode = m_Nodes[m_Relations[iMCopRel].m_TargetNodeNo].RusNode;
	assert( iRusNode!=-1 ); // ������� �������
	if( iRusNode==-1 )
		return;

// ��� ����
	CEngSemNode newNode;
	int iSubjPattern = -1, iObjPattern = -1;

	if( iSubjRel>=0 )
	{		
		CreateHaveSemNode(newNode,iSubjPattern,iObjPattern);
		#ifdef _DEBUG
			assert( (iObjPattern!=-1) && (iSubjPattern!=-1) );
			assert( newNode.m_Words.size()==1 );
		#endif
		if ((iObjPattern==-1) || (iSubjPattern==-1) )
			return;

		newNode.RusNode = engNode.RusNode;
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

		long wNo = m_Nodes[iVerbNode].GetMinWordNo();
		FreeWordNo(wNo);
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes[iEngNode] = newNode;

		m_Relations[iSubjRel].m_bInterpreted = true;
		m_Relations[iSubjRel].m_Valency = newNode.m_Vals[0];
		m_Relations[iSubjRel].m_Pattern = newNode.m_Patterns[iSubjPattern];
		m_Relations[iSubjRel].m_SynReal.m_Cortege = newNode.m_Patterns[iSubjPattern].m_GramCorteges[0];

		m_Relations[iSubjRel].m_Position = "<";
		m_Relations[iSubjRel].m_PosType = FromAlgorithmPosType;
	}
//
	else
	{
		CreateBeSemNode(newNode,iSubjPattern,iObjPattern);
		assert( (iObjPattern!=-1) && (iSubjPattern!=-1) );
		assert( newNode.m_Words.size()==1 );

		newNode.RusNode = engNode.RusNode;
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

		long wNo = m_Nodes[iVerbNode].GetMinWordNo();
		FreeWordNo(wNo);
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes[iEngNode] = newNode;
		
		CEngSemNode newNode1;
		CreateSimpleEnglNode("there",newNode1,0,true); // there
		assert(newNode1.m_Words.size()==1);

		newNode1.RusNode = -1;
		newNode1.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

		FreeWordNo(wNo);
		newNode1.m_Words[0].m_WordNo = wNo;
		m_Nodes.push_back(newNode1);

		// �������� ��������� �� be � there 
		// ������ ��� ��������� ����������
		CEngSemRelation newRel1(CValency("ModalCopul",A_C),iEngNode,m_Nodes.size()-1,"");
		newRel1.m_bInterpreted = true;
		newRel1.m_Valency = m_Nodes[iEngNode].m_Vals[iSubjPattern];
		newRel1.m_SynReal.m_Cortege = m_Nodes[iEngNode].m_Patterns[iSubjPattern].m_GramCorteges[iSubjPattern];
		newRel1.m_Pattern  = m_Nodes[iEngNode].m_Patterns[iSubjPattern];
		newRel1.m_Position = "<<";
		newRel1.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(newRel1); // be->there
	}

//
	m_Relations[iVerbRel].m_bInterpreted = false;
	m_Relations[iVerbRel].m_SynReal.SetEmpty();
	m_Relations[iVerbRel].m_SyntacticRelation.erase();
	m_Relations[iVerbRel].m_Valency.m_RelationStr = "ModalCopul";

// �������� ������� ModalCopul
	m_Relations[iMCopRel].m_Position = "<";
	m_Relations[iMCopRel].m_PosType = FromAlgorithmPosType;

// �������� �����
	if( iSubjRel>=0 )
	{
		CEngSemNode& subNode = m_Nodes[m_Relations[iSubjRel].m_TargetNodeNo];
		if( subNode.m_NodeType == MNA )
			m_Nodes[iEngNode].AddOneGrammemRich( ePlural);
		else
			m_Nodes[iEngNode].AddGrammemsRich( subNode.GetGrammemsRich() & eAllNumbers );
	}
	else
		m_Nodes[iEngNode].AddOneGrammemRich( eSingular);

// �������� �����
	TransferTimeGrammems(RusStr.GetNode(iRusNode),m_Nodes[iEngNode],"ApplyModalCopulRule");
	if( m_Nodes[iEngNode].m_Words[0].GetTense() == zero_tn )
		m_Nodes[iEngNode].m_Words[0].SetTense(present_smp_tn,"ApplyModalCopulRule");
	
	m_Nodes[m_Relations[iMCopRel].m_TargetNodeNo].m_Words[0].SetTense(zero_tn,"ApplyModalCopulRule");
        
	
// �������� ��������	
	if( m_Relations[iMCopRel].m_SynReal.m_Preps.size() )
	{
		int iMainWord = m_Nodes[m_Relations[iMCopRel].m_SourceNodeNo].m_MainWordNo;
		if( iMainWord != -1 )
		{
			m_Nodes[m_Relations[iMCopRel].m_SourceNodeNo].m_Words[iMainWord].m_PosleLog.m_PosleLogPrep = m_Relations[iMCopRel].m_SynReal.m_Preps[0];
			m_Relations[iMCopRel].m_SynReal.m_Preps.clear();
		}
	}

// ����� ������:
	CEngSemRelation newRel(CValency("ModalCopul",A_C),iEngNode,m_Relations[iMCopRel].m_TargetNodeNo,"");
	newRel.m_Position = ">>";
	newRel.m_PosType = FromAlgorithmPosType;
	m_Relations.push_back(newRel); // �opul->Word
	EraseRelation(iMCopRel);
}

/////////////////////////////////////////////////////////////////////////////
// ApplyNoRule() "���" (no 1) 
// ex. "� ���� ��� �����" | "��� �����"
// --> ������� have | there be

void CEngSemStructure::ApplyNoRule(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	if( m_Nodes[iEngNode].GetType()!=Aoss && m_Nodes[iEngNode].GetType()!=EngCollocRoss )
		return;
	if( !HasALG(m_Nodes[iEngNode].GetType(),m_Nodes[iEngNode].GetUnitNo(),"AL_there") )
		return;
	string dbg_str = GetRoss(Aoss)->GetEntryStr(m_Nodes[iEngNode].GetUnitNo());
	int    dbg_num = GetRoss(Aoss)->GetUnitMeanNum(m_Nodes[iEngNode].GetUnitNo());
//
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;
//
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	if( outRels.size() == 0 )
		return;
//
	int iSubRel = -1, iObjRel = -1, iMoreRel = -1;
	for( int i=0; i<outRels.size(); i++ )
	{
		if( iSubRel<0 )
		{
			if	(		m_Relations[outRels[i]].m_Valency.m_RelationStr == "SUB" 
					||	m_Relations[outRels[i]].m_Valency.m_RelationStr == "F-ACT" 
				)			
			{
				iSubRel = outRels[i];			
				continue;
			}
		}
		
		if( iObjRel<0 )
		{
				if	(		m_Relations[outRels[i]].m_Valency.m_RelationStr == "OBJ" 
						||	m_Relations[outRels[i]].m_Valency.m_RelationStr == "S-ACT" 
					)			
				{
				int iRusObj = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].RusNode;
				if( iRusObj<0 )
					continue;
				if( RusStr.IsNounOrMNANoun(iRusObj) )
					iObjRel = outRels[i];
			}
		}
		if (iMoreRel==-1)
		{
			const CEngSemNode& N = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo];
			if (N.IsLemma("many"))
				iMoreRel = outRels[i];
		};
	}
// ������ Subj | Obj
	if( iSubRel<0 && iObjRel<0 )
		return;

	// �������� � �������  ���������, ����� ��������� ����� ��������� ���������� 
	// � ����������� "������ �� ����"
	if( iObjRel!=-1 )
	{
		m_Relations[iObjRel].m_RusRel = -1; 
	};


	//  �����, �����  more �� ���������� �� "no":  "I have no more hope"
	if (iMoreRel != -1)
	{
		m_Relations[iMoreRel].m_PosType = FromAlgorithmPosType;
		m_Relations[iMoreRel].m_Position = ">>>";
	};



// ���� ������� �������� "��" 
	m_Nodes[iEngNode].m_RelOperators.clear();

	if( iSubRel>=0 )
	{		
		CEngSemNode newNode;
		int iSubjPattern = -1, iObjPattern = -1;
		CreateHaveSemNode(newNode,iSubjPattern,iObjPattern);
		assert( (iObjPattern!=-1) && (iSubjPattern!=-1) );
		assert( newNode.m_Words.size()==1 );

		newNode.RusNode = iRusNode;
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
		
		m_Relations[iSubRel].m_Pattern = newNode.m_Patterns[iSubjPattern];
		m_Relations[iSubRel].m_bInterpreted = true;

		long wNo = m_Nodes[iEngNode].GetMinWordNo();
		FreeWordNo(wNo);
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes.push_back(newNode);	// have	

		m_Relations[iSubRel].m_Valency = newNode.m_Vals[0];
		m_Relations[iSubRel].m_SynReal.m_Cortege = newNode.m_Patterns[iSubjPattern].m_GramCorteges[0];
		m_Relations[iSubRel].m_SynReal.m_Preps.clear(); // ��������
		m_Relations[iSubRel].m_SourceNodeNo = m_Nodes.size()-1; // have->Subj

		CEngSemRelation newRel(CValency("ApplyNoRule",A_C),m_Nodes.size()-1,iEngNode,"");
		newRel.m_Position = ">";
		newRel.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(newRel); // have->no
	}
//
	else
	{
		CEngSemNode newNode1;
		CreateSimpleEnglNode("there",newNode1,0,true); // there
		assert(newNode1.m_Words.size()==1);

		newNode1.RusNode = -1;
		newNode1.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

		long wNo = m_Nodes[iEngNode].GetMinWordNo();
		FreeWordNo(wNo);
		newNode1.m_Words[0].m_WordNo = wNo;
		m_Nodes.push_back(newNode1);
//
		CEngSemNode newNode2 = m_Nodes[iEngNode];

		int iSubjPattern = -1, iObjPattern = -1;
		CreateBeSemNode(newNode2,iSubjPattern,iObjPattern); // be
		assert( (iObjPattern!=-1) && (iSubjPattern!=-1) );
		assert( newNode2.m_Words.size()==1 );

		newNode2.RusNode = iRusNode;
		newNode2.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
		newNode2.m_RelOperators.clear();

		FreeWordNo(wNo+1);
		newNode2.m_Words[0].m_WordNo = wNo+1;
		m_Nodes.push_back(newNode2);
//
		CEngSemRelation newRel1(CValency("ApplyNoRule",A_C),m_Nodes.size()-1,m_Nodes.size()-2,"");
		newRel1.m_bInterpreted = true;
		newRel1.m_Position = "<<";
		newRel1.m_PosType = FromAlgorithmPosType;
		newRel1.m_SynReal.m_Cortege = newNode2.m_Patterns[iSubjPattern].m_GramCorteges[0];
		newRel1.m_Valency = newNode2.m_Vals[0];
		m_Relations.push_back(newRel1); // be->there

		CEngSemRelation newRel2(CValency("ApplyNoRule",A_C),m_Nodes.size()-1,iEngNode,"");
		newRel2.m_Position = ">";
		newRel2.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(newRel2); // be->no
	}

	int iNewNode = m_Nodes.size() - 1; // have | be

	if( iSubRel>=0 ) // �������� ����� �� ��������
	{
		CEngSemNode& subNode = m_Nodes[m_Relations[iSubRel].m_TargetNodeNo];
		if( subNode.m_NodeType == MNA )
				m_Nodes[iNewNode].AddOneGrammemRich( ePlural);
		else if( subNode.m_MainWordNo!=-1 )
			m_Nodes[iNewNode].AddGrammemsRich( subNode.GetGrammemsRich() & eAllNumbers );
		else
			m_Nodes[iNewNode].AddOneGrammemRich( eSingular);
	}
	else // �������� ����� �� �������
	{
		CEngSemNode& objNode = m_Nodes[m_Relations[iObjRel].m_TargetNodeNo];
		if( objNode.m_NodeType == MNA )
				m_Nodes[iNewNode].AddOneGrammemRich(ePlural);
		else if( objNode.m_MainWordNo!=-1 )
			m_Nodes[iNewNode].AddGrammemsRich( objNode.GetGrammemsRich() & eAllNumbers );
		else
			m_Nodes[iNewNode].AddOneGrammemRich(eSingular);
	}

// �������� �����
	TransferTimeGrammems(RusStr.GetNode(iRusNode),m_Nodes[iNewNode],"ApplyNoRule");
	if( m_Nodes[iNewNode].m_Words[0].GetTense() == zero_tn )
		m_Nodes[iNewNode].m_Words[0].SetTense(present_smp_tn,"ApplyNoRule");
	m_Nodes[iEngNode].m_Words[0].SetTense(zero_tn,"ApplyNoRule");

// �����
	PrintNodes();
	PrintRelations();
	MoveMeaningRelsToNewNode(iEngNode,iNewNode);

// ��� �� �����,�� �����������
// � ���� ��� ������
	if( iObjRel<0 )
		return;
	CEngSemNode& objNode = m_Nodes[m_Relations[iObjRel].m_TargetNodeNo];

	bool negMna = false;
	if( objNode.RusNode != -1 &&
		RusStr.GetNode(objNode.RusNode).m_NodeType == MNA &&
		RusStr.GetNode(objNode.RusNode).m_MainWordNo != -1 &&
		RusStr.GetNode(objNode.RusNode).GetWord(RusStr.GetNode(objNode.RusNode).m_MainWordNo).m_Lemma == "��" )
	{
		negMna = true;
	}
	
	vector<long> outChilds;
	GetChildNodes(iEngNode,outChilds);
	bool negChild = false;
	PrintRelations();
	for( int i=0; i<outChilds.size(); i++ )
	{
		if( HasNeg(m_Nodes[outChilds[i]].GetUnitNo(),m_Nodes[outChilds[i]].GetType()) )
		{
			negChild = true;
			break;
		}
	}

	if( !negMna && !negChild )
		return;

// ���� ������� ��� ���� ����
	vector<long> inRels;
	GetIncomingRelations(iEngNode,inRels,false);
	assert( inRels.size() == 1 );
	m_Relations.erase(m_Relations.begin() +inRels[0]); // be|have -> no
	MoveRelations(iEngNode,iNewNode);

// ���� ������� �������� "��" ��� ApplyTwoNegationsRule()
	if( negChild )
	{
		m_Nodes[iNewNode].RusNode = m_Nodes[iEngNode].RusNode;
		m_Nodes[iNewNode].m_RelOperators.push_back("��");
	}
	
	DelNode(iEngNode);
}

/////////////////////////////////////////////////////////////////////////////
// ApplyBeRule() be 4
// ��. "���� ����� ������ �����"
// --> ������� "there"

void CEngSemStructure::ApplyBeRule(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	if( m_Nodes[iEngNode].GetType() != Aoss )
		return;
	string dbg_str = GetRoss(Aoss)->GetEntryStr(m_Nodes[iEngNode].GetUnitNo());
	int    dbg_num = GetRoss(Aoss)->GetUnitMeanNum(m_Nodes[iEngNode].GetUnitNo());
	if( dbg_str != "be" || dbg_num != 4 )
		return;
//
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;
//
	int iRusClau = RusStr.GetNode(iRusNode).m_ClauseNo;
	if( RusStr.IsQuestionClause(iRusClau) )
		return;
	/*
		�� ����� "��� ���� ����" there ��� ����, ������� ��������� ��� �� ����.
	*/
	int RelNo = GetOutRelationByWord(iEngNode, "there");
	if (RelNo != -1 )
	{
		m_Relations[RelNo].m_Position = "<<";
		m_Relations[RelNo].m_PosType = FromAlgorithmPosType;
		m_Relations[RelNo].m_bReverseRel = false;

		const CEngSemNode& BeNode = m_Nodes[m_Relations[RelNo].m_SourceNodeNo];
		m_Relations[RelNo].m_SynReal.m_Cortege = BeNode.m_Patterns[0].m_GramCorteges[0];
		m_Relations[RelNo].m_Valency = BeNode.m_Vals[0];
		
	}
	else
	{
		CEngSemNode newNode;
		CreateSimpleEnglNode("there",newNode,0,true);
		assert( newNode.m_Words.size()==1 );

		newNode.RusNode = -1;
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

		long wNo = m_Nodes[iEngNode].GetMinWordNo();
		FreeWordNo(wNo);
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes.push_back(newNode);
	//
		CEngSemRelation newRel(CValency("ApplyBeRule",A_C),iEngNode,m_Nodes.size()-1,"");
		newRel.m_Position = "<<";
		newRel.m_PosType = FromAlgorithmPosType;

		const CEngSemNode& BeNode = m_Nodes[iEngNode];
		newRel.m_SynReal.m_Cortege = BeNode.m_Patterns[0].m_GramCorteges[0];
		newRel.m_Valency = BeNode.m_Vals[0];

		m_Relations.push_back(newRel); // be->there
	};

// ���� ��� Subj, ���� ���������� ����� ��� be � �������, ��� ��� ������ �� Subj
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		int iRusRel = m_Relations[outRels[i]].m_RusRel;
		if( iRusRel == -1 )
			continue;
		if( !RusStr.IsRusSubj(iRusRel) )
			continue;
		int iRusNode = RusStr.GetRelation(iRusRel)->m_TargetNodeNo;
		int iSubNode = GetEngNodeByRusNode(iRusNode);
		if( iSubNode == -1 )
			continue;

		CEngSemNode& subNode = m_Nodes[iSubNode];
		m_Nodes[iEngNode].DeleteGrammemsRich(eAllNumbers);
		if( subNode.m_NodeType == MNA )
			m_Nodes[iEngNode].AddOneGrammemRich(ePlural);
		else if( subNode.m_MainWordNo!=-1 )
			m_Nodes[iEngNode].AddGrammemsRich( subNode.GetGrammemsRich() & eAllNumbers );
		else
			m_Nodes[iEngNode].AddOneGrammemRich(eSingular);

		m_Relations[outRels[i]].m_RusRel = -1;
		//m_Relations[outRels[i]].m_SynReal.m_Cortege.SetItem(0,-1);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ApplyKeepRule() __vce
// ex. "��� ��� ������� ���������� �� ����"
// --> ��������� keep � ������ � �������� ������� ��������

void CEngSemStructure::ApplyKeepRule(int iEngNode)
{
	if (!m_Nodes[iEngNode].HasRelOperator("�������")) return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;
	int iSemMainWord = RusStr.GetNode(iRusNode).m_MainWordNo;
	if( iSemMainWord == -1 )
		return;
	if( !RusStr.IsVerbOrMNAVerb(m_Nodes[iEngNode].RusNode) )
		return;

   // ���� VERB � ����������...
	if( m_Nodes[iEngNode].HasRelOperator("��") )
	{
		CEngSemNode newNode;
		CreateSimpleEnglNode("still",newNode,0,true);
		assert( newNode.m_Words.size()==1 );
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
		long wNo = m_Nodes[iEngNode].GetMinWordNo();
		FreeWordNo(wNo);
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes.push_back(newNode);

		CEngSemRelation newRel(CValency("ACT",A_C,GetRossHolder(Aoss)),iEngNode,m_Nodes.size()-1,"");		
		newRel.m_bReverseRel = false;
		newRel.m_Position = "<";
		newRel.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(newRel);
		return;
	}

// ������� �� VERB ���� KEEP
	CEngSemNode keepNode;
	CreateSimpleEnglNodeByOldNode("keep",keepNode,eVERB,false,m_Nodes[iEngNode]);
	assert( keepNode.m_Words.size()==1 );
	keepNode.RusNode = iRusNode;
	TransferTimeGrammems(RusStr.GetNode(iRusNode),keepNode, "ApplyKeepRule");

	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	keepNode.m_Words[0].m_WordNo = wNo;
	keepNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
	m_Nodes.push_back(keepNode);

// ���������� Subj (���� ����) � �������� �����
	int iSubRel = -1;
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		if( IsSubj(m_Relations[outRels[i]]) )
		{
			iSubRel = outRels[i];
			break;
		}
	}

	if( iSubRel != -1 )
	{
		m_Relations[iSubRel].m_SourceNodeNo = m_Nodes.size() - 1;

		int iSubNode = m_Relations[iSubRel].m_TargetNodeNo;
		CEngSemNode& subNode = m_Nodes[iSubNode];
		if( subNode.m_NodeType == MNA )
				m_Nodes[m_Nodes.size() - 1].AddOneGrammemRich(ePlural);
		else if( subNode.m_MainWordNo!=-1 )
			m_Nodes[m_Nodes.size() - 1].AddGrammemsRich(subNode.GetGrammemsRich() & eAllNumbers);
		else
			m_Nodes[m_Nodes.size() - 1].AddOneGrammemRich(eSingular);
	}

// �������� ����� 
	CEngSemRelation newRel(CValency("ApplyKeepRule",A_C,GetRossHolder(Aoss)),m_Nodes.size()-1,iEngNode,"");
	newRel.m_bReverseRel = false;
	newRel.m_bInterpreted = false;
	newRel.m_Position = ">";
	newRel.m_PosType = FromAlgorithmPosType;
	m_Relations.push_back(newRel);

	// �������� ��������
	m_Nodes[iEngNode].m_Words[iSemMainWord].SetTense(gerund_tn,"ApplyKeepRule");


// ���� ��������� �������� ���� "AND" ������ � ������������ ��������� ��� ��
	outRels.clear();
	GetOutcomingRelations(iEngNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		if( m_Relations[outRels[i]].m_Valency.m_RelationStr != "AND" )
			continue;
		int iNode = m_Relations[outRels[i]].m_TargetNodeNo;
		if( m_Nodes[iNode].GetType()==NoneRoss )
			continue;
		if( GetRossHolder(m_Nodes[iNode].GetType())->HasFieldValue("SF","MODL",m_Nodes[iNode].GetUnitNo()) )			
			break;
		if( m_Nodes[iNode].HasRelOperator("��") )
			break;
		if( !GetRossHolder(m_Nodes[iNode].GetType())->HasFieldValue("GF","VERB",m_Nodes[iNode].GetUnitNo()) )
			continue;
		m_Nodes[iNode].m_Words[iSemMainWord].SetTense(gerund_tn,"ApplyKeepRule");
	}

// �����
	MoveMeaningRelsToNewNode(iEngNode,m_Nodes.size() -1);
}

/////////////////////////////////////////////////////////////////////////////
// ApplyALG_it() "��� �������" | "��� ������ ������"
// --> ������� "it"   (it seem | it hurts)

void CEngSemStructure::ApplyALG_it(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;

	if( m_Nodes[iEngNode].GetType()!=Aoss && m_Nodes[iEngNode].GetType()!=EngCollocRoss )
		return;
	if( !HasALG(m_Nodes[iEngNode].GetType(),m_Nodes[iEngNode].GetUnitNo(),"AL_it") )
		return;

	CSemPattern SubjPattern;
	if( !FindSubjPattern(m_Nodes[iEngNode], SubjPattern))
		return;

	//assert( GetEngPOSFromArticle(m_Nodes[iEngNode].GetUnitNo(),m_Nodes[iEngNode].GetType())== _QM(eVERB) );

	if( m_Nodes[iEngNode].m_NodeType == Copul )
		return;
	if( is_not_finite_form(m_Nodes[iEngNode].GetTense()) )
		return;

	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		if( IsSubj(m_Relations[outRels[i]]) )
			return;
	}
	
	CEngSemNode newNode;
	CreateSimpleEnglNode("it",newNode,0,true);
	assert( newNode.m_Words.size()==1 );
	newNode.m_Words[0].AddFormGrammem(eSingular);

	newNode.RusNode = -1;
	newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
	
	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;
	m_Nodes.push_back(newNode);

	CEngSemRelation newRel(CValency("AL_it",A_C),iEngNode,m_Nodes.size()-1,"");
	newRel.m_SynReal.m_Cortege  = SubjPattern.m_GramCorteges[0];
	newRel.m_Valency = SubjPattern.m_PatternValency;
	newRel.m_Pattern = SubjPattern;
	newRel.m_Position = "<";
	newRel.m_PosType = FromAlgorithmPosType;
	
	m_Relations.push_back(newRel);
	assert (IsSubj(m_Relations.back()));

// �������� ����� � �����
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode != -1 )
		TransferTimeGrammems(RusStr.GetNode(iRusNode),m_Nodes[iEngNode],"ApplyALG_it");
	if( m_Nodes[iEngNode].m_Words[0].GetTense() == zero_tn )
		m_Nodes[iEngNode].m_Words[0].SetTense(present_smp_tn,"ApplyALG_it");
	m_Nodes[iEngNode].AddOneGrammemRich(eSingular);
}

/////////////////////////////////////////////////////////////////////////////
// ApplyALG_compl_obj() <AL5|AL6> (�����������)
// ex. "�� �����, ����� �� �����" | "�� ������, ��� ��� �����"
// --> ������� "��" subj, �������� "�����" ��������� (5?6) �������� "to"

void CEngSemStructure::ApplyALG_compl_obj(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	if( m_Nodes[iEngNode].GetType()!=Aoss && m_Nodes[iEngNode].GetType()!=EngCollocRoss )
		return;
	if( !HasALG(m_Nodes[iEngNode].GetType(),m_Nodes[iEngNode].GetUnitNo(),"AL_compl_obj_to") &&
		!HasALG(m_Nodes[iEngNode].GetType(),m_Nodes[iEngNode].GetUnitNo(),"AL_compl_obj") )
		return;

// �������� ������� ��������� ����������� ����� "AIM"|"CONTEN" � �������	
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	int i=0;
	for( ; i<outRels.size(); i++ )
	{
		if( m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].m_ClauseNo == 
			m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo )
			continue;

		if	(			!(m_Relations[outRels[i]].m_Valency.m_RelationStr == "AIM") 
					&&	!(m_Relations[outRels[i]].m_Valency.m_RelationStr == "CONTEN") 
			)			
		continue;

		int iEnode = m_Relations[outRels[i]].m_TargetNodeNo;
		int iRnode = m_Nodes[iEnode].RusNode;
		if( iRnode==-1 )
			continue;
		int iRword = RusStr.GetNode(iRnode).m_MainWordNo;
		if( iRword==-1 )
			continue;

		if( RusStr.GetNode(iRnode).GetWord(iRword).HasPOS(VERB) )
			break;
	}
	if( i>=outRels.size() )
		return;
//
	assert( m_Nodes[iEngNode].GetType() == Aoss );
	bool bUseTo = HasALG(m_Nodes[iEngNode].GetType(),m_Nodes[iEngNode].GetUnitNo(),"AL_compl_obj_to");
	
	int iNodeVerb = m_Relations[outRels[i]].m_TargetNodeNo;
	CEngSemNode& rNodeVerb = m_Nodes[iNodeVerb];
	int iRelToVerb = outRels[i];
	CEngSemRelation& rRelToVerb = m_Relations[iRelToVerb];

// �������� ������������ ���������
	outRels.clear();
	GetOutcomingRelations(iNodeVerb,outRels);
	for( i=0; i<outRels.size(); i++ )
	{
		if( m_Relations[outRels[i]].m_Valency.m_RelationStr != "AND" )
			continue;
		int iNode = m_Relations[outRels[i]].m_TargetNodeNo;
		if( m_Nodes[iNode].GetType()==NoneRoss )
			continue;
		if( !GetRossHolder(m_Nodes[iNode].GetType())->HasFieldValue("GF","VERB",m_Nodes[iNode].GetUnitNo()) )
			continue;
		if( GetRossHolder(m_Nodes[iNode].GetType())->HasFieldValue("SF","MODL",m_Nodes[iNode].GetUnitNo()) )			
			return;
		if( m_Nodes[iNode].HasRelOperator("��") )
			return;
	}

	// �������� ������� ��������� ����� "SUB"	
	i=0;
	for( ; i<outRels.size(); i++ )
	{
		if( IsSubj(m_Relations[outRels[i]]) )
			break;
	}
	if( i>=outRels.size() )
		return;
	int iRelToSubj = outRels[i];

	/*
	 ���� � ����������� ���� ���������, ����� �� ���� 
	 ��������� ��������, ��������� ����������� c that ������ ���������,
	 � complex object �� ������.
	 ��������, �������, ������ ������� 
	 "We saw this black or white or yellow man quickly or slowly run"
	 ������, ���� � �������.
	 �������
	*/
	if (GetOutcomingRelationsCount(m_Relations[outRels[i]].m_TargetNodeNo, false) > 0)
		return;


// Ok
	MakeEngInfinitive(iNodeVerb);
	
	rRelToVerb.m_SynReal.SetEmpty();
	rNodeVerb.m_SynReal.m_Preps.clear();
	rRelToVerb.m_Pattern.SetEmpty();
	rRelToVerb.m_Valency.m_RelationStr = "Compl_obj";
	rRelToVerb.m_RusRel = -1;

	if( !bUseTo )
		m_Nodes[iNodeVerb].m_bNotUseTo = true;

	rRelToVerb.m_bInterpreted = false;
	rRelToVerb.m_Type = NotBetweenClauses;

// ���� ��������� ��������� ���� "AND" ������ � ������������ ��������� ��� ��
	outRels.clear();
	GetOutcomingRelations(iNodeVerb,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		if( m_Relations[outRels[i]].m_Valency.m_RelationStr != "AND" )
			continue;
		int iNode = m_Relations[outRels[i]].m_TargetNodeNo;
		if( m_Nodes[iNode].GetType()==NoneRoss )
			continue;
		if( GetRossHolder(m_Nodes[iNode].GetType())->HasFieldValue("SF","MODL",m_Nodes[iNode].GetUnitNo()) )			
			break;
		if( m_Nodes[iNode].HasRelOperator("��") )
			break;
		if( !GetRossHolder(m_Nodes[iNode].GetType())->HasFieldValue("GF","VERB",m_Nodes[iNode].GetUnitNo()) )
			continue;
		MakeEngInfinitive(iNode);
		if( !bUseTo )
		{
			m_Relations[outRels[i]].m_SynReal.m_Preps.clear();
			m_Nodes[iNode].m_bNotUseTo = true;
		}
	}

// Ok
	CEngSemRelation& rRelToSubj =  m_Relations[iRelToSubj];	
	rRelToSubj.m_SourceNodeNo = iEngNode;
	rRelToSubj.m_RusRel = -1;
	rRelToSubj.m_bInterpreted = false;
	rRelToSubj.m_Valency.m_RelationStr = "Compl_obj";
	rRelToSubj.m_SynReal.m_Cortege.SetItem(0, -1);
	rRelToSubj.m_Position = ">";	
	rRelToSubj.m_PosType = FromAlgorithmPosType;	

// ��������� Subj � ������ � ���������� ������ 
	int iNodeSubj = m_Relations[iRelToSubj].m_TargetNodeNo;
	m_Nodes[iNodeSubj].m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
	outRels.clear();
	GetOutcomingRelations(iNodeSubj,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
	}

// "�����������"
	ChangeGrammemsForDirObj(iNodeSubj); // she->her
}

/////////////////////////////////////////////////////////////////////////////
